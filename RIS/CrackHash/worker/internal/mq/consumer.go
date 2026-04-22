package mq

import (
	"context"
	"encoding/json"
	"fmt"
	"sync"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/google/uuid"
)

const (
	TaskExchangeName = "crackhash.tasks"
	TaskQueueName    = "crackhash.tasks.worker"
	TaskRoutingKey   = "crack-task"
)

type CrackTaskMessage struct {
	RequestId  string `json:"requestId"`
	Hash       string `json:"hash"`
	MaxLength  int32  `json:"maxLength"`
	Alphabet   string `json:"alphabet"`
	PartNumber int32  `json:"partNumber"`
	PartCount  int32  `json:"partCount"`
}

type TaskHandler func(ctx context.Context, msg CrackTaskMessage) ([]string, error)

type TaskConsumer struct {
	conn       *amqp.Connection
	channel    *amqp.Channel
	publisher  *Publisher
	handler    TaskHandler
	mu         sync.Mutex
	url        string
	done       chan struct{}
}

func NewTaskConsumer(url string, publisher *Publisher, handler TaskHandler) (*TaskConsumer, error) {
	c := &TaskConsumer{
		publisher: publisher,
		handler:   handler,
		url:       url,
		done:      make(chan struct{}),
	}
	if err := c.connect(); err != nil {
		return nil, err
	}
	go c.handleReconnect()
	return c, nil
}

func (c *TaskConsumer) connect() error {
	conn, err := amqp.Dial(c.url)
	if err != nil {
		return fmt.Errorf("amqp dial: %w", err)
	}

	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return fmt.Errorf("amqp channel: %w", err)
	}

	if err := ch.Qos(1, 0, false); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("qos: %w", err)
	}

	if err := ch.ExchangeDeclare(TaskExchangeName, "direct", true, false, false, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("exchange declare: %w", err)
	}

	if _, err := ch.QueueDeclare(TaskQueueName, true, false, false, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("queue declare: %w", err)
	}

	if err := ch.QueueBind(TaskQueueName, TaskRoutingKey, TaskExchangeName, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("queue bind: %w", err)
	}

	c.conn = conn
	c.channel = ch
	return nil
}

func (c *TaskConsumer) handleReconnect() {
	for {
		select {
		case <-c.done:
			return
		case err, ok := <-c.conn.NotifyClose(make(chan *amqp.Error, 1)):
			if !ok {
				return
			}
			logger.Log.Warn("RabbitMQ connection lost, reconnecting...", "error", err)
			for {
				select {
				case <-c.done:
					return
				default:
				}
				c.mu.Lock()
				if connectErr := c.connect(); connectErr != nil {
					c.mu.Unlock()
					logger.Log.Error("RabbitMQ reconnect failed, retrying in 5s", "error", connectErr)
					time.Sleep(5 * time.Second)
					continue
				}
				c.mu.Unlock()
				logger.Log.Info("RabbitMQ reconnected successfully")
				break
			}
		}
	}
}

func (c *TaskConsumer) Start(ctx context.Context) {
	for {
		c.mu.Lock()
		ch := c.channel
		c.mu.Unlock()

		if ch == nil {
			select {
			case <-ctx.Done():
				return
			case <-time.After(2 * time.Second):
				continue
			}
		}

		deliveries, err := ch.Consume(TaskQueueName, "", false, false, false, false, nil)
		if err != nil {
			logger.Log.Error("Failed to start consuming, retrying in 5s", "error", err)
			select {
			case <-ctx.Done():
				return
			case <-time.After(5 * time.Second):
				continue
			}
		}

		for {
			select {
			case <-ctx.Done():
				return
			case msg, ok := <-deliveries:
				if !ok {
					goto reConsume
				}
				c.handleMessage(ctx, msg)
			}
		}

	reConsume:
		select {
		case <-ctx.Done():
			return
		case <-time.After(2 * time.Second):
		}
	}
}

func (c *TaskConsumer) handleMessage(ctx context.Context, msg amqp.Delivery) {
	var taskMsg CrackTaskMessage
	if err := json.Unmarshal(msg.Body, &taskMsg); err != nil {
		logger.Log.Error("Failed to unmarshal task message", "error", err)
		msg.Nack(false, false)
		return
	}

	if _, err := uuid.Parse(taskMsg.RequestId); err != nil {
		logger.Log.Error("Invalid requestId in task message", "requestId", taskMsg.RequestId, "error", err)
		msg.Nack(false, false)
		return
	}

	results, err := c.handler(ctx, taskMsg)
	if err != nil {
		logger.Log.Error("Error processing task, requeueing",
			"requestId", taskMsg.RequestId,
			"partNumber", taskMsg.PartNumber,
			"error", err,
		)
		msg.Nack(false, true)
		return
	}

	if err := c.publisher.Publish(ctx, FoundWordsMessage{
		RequestId:  taskMsg.RequestId,
		PartNumber: taskMsg.PartNumber,
		Answers:    results,
	}); err != nil {
		logger.Log.Error("Error publishing results, requeueing task",
			"requestId", taskMsg.RequestId,
			"partNumber", taskMsg.PartNumber,
			"error", err,
		)
		msg.Nack(false, true)
		return
	}

	logger.Log.Info("Task processed and results published",
		"requestId", taskMsg.RequestId,
		"partNumber", taskMsg.PartNumber,
	)
	msg.Ack(false)
}

func (c *TaskConsumer) Close() error {
	close(c.done)
	c.mu.Lock()
	defer c.mu.Unlock()
	if c.channel != nil {
		c.channel.Close()
	}
	if c.conn != nil {
		return c.conn.Close()
	}
	return nil
}
