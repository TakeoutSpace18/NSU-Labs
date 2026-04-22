package mq

import (
	"context"
	"encoding/json"
	"fmt"
	"sync"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/google/uuid"
)

const (
	ExchangeName = "crackhash.results"
	QueueName    = "crackhash.results.manager"
	RoutingKey   = "found-words"
)

type FoundWordsMessage struct {
	RequestId  string   `json:"requestId"`
	PartNumber int32    `json:"partNumber"`
	Answers    []string `json:"answers"`
}

type FoundWordsHandler func(requestId uuid.UUID, partNumber int32, answers []string)

type Consumer struct {
	conn    *amqp.Connection
	channel *amqp.Channel
	handler FoundWordsHandler
	mu      sync.Mutex
	url     string
	done    chan struct{}
}

func NewConsumer(url string, handler FoundWordsHandler) (*Consumer, error) {
	c := &Consumer{
		handler: handler,
		url:     url,
		done:    make(chan struct{}),
	}
	if err := c.connect(); err != nil {
		logger.Log.Warn("RabbitMQ not available on startup, will retry in background", "error", err)
	}
	go c.handleReconnect()
	return c, nil
}

func (c *Consumer) connect() error {
	conn, err := amqp.Dial(c.url)
	if err != nil {
		return fmt.Errorf("amqp dial: %w", err)
	}

	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return fmt.Errorf("amqp channel: %w", err)
	}

	if err := ch.Qos(10, 0, false); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("qos: %w", err)
	}

	if err := ch.ExchangeDeclare(ExchangeName, "direct", true, false, false, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("exchange declare: %w", err)
	}

	if _, err := ch.QueueDeclare(QueueName, true, false, false, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("queue declare: %w", err)
	}

	if err := ch.QueueBind(QueueName, RoutingKey, ExchangeName, false, nil); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("queue bind: %w", err)
	}

	c.conn = conn
	c.channel = ch
	return nil
}

func (c *Consumer) handleReconnect() {
	for {
		c.mu.Lock()
		conn := c.conn
		c.mu.Unlock()

		if conn == nil {
			c.mu.Lock()
			if err := c.connect(); err != nil {
				c.mu.Unlock()
				logger.Log.Error("RabbitMQ connect failed, retrying in 5s", "error", err)
				select {
				case <-c.done:
					return
				case <-time.After(5 * time.Second):
					continue
				}
			}
			c.mu.Unlock()
			logger.Log.Info("RabbitMQ connected successfully")
		}

		c.mu.Lock()
		conn = c.conn
		c.mu.Unlock()

		select {
		case <-c.done:
			return
		case err, ok := <-conn.NotifyClose(make(chan *amqp.Error, 1)):
			if !ok {
				return
			}
			logger.Log.Warn("RabbitMQ connection lost, reconnecting...", "error", err)
			c.mu.Lock()
			c.conn = nil
			c.channel = nil
			c.mu.Unlock()
		}
	}
}

func (c *Consumer) Start(ctx context.Context) {
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

		deliveries, err := ch.Consume(QueueName, "", false, false, false, false, nil)
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
				c.handleMessage(msg)
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

func (c *Consumer) handleMessage(msg amqp.Delivery) {
	var foundWords FoundWordsMessage
	if err := json.Unmarshal(msg.Body, &foundWords); err != nil {
		logger.Log.Error("Failed to unmarshal message", "error", err)
		msg.Nack(false, false)
		return
	}

	requestId, err := uuid.Parse(foundWords.RequestId)
	if err != nil {
		logger.Log.Error("Invalid requestId in message", "requestId", foundWords.RequestId, "error", err)
		msg.Nack(false, false)
		return
	}

	c.handler(requestId, foundWords.PartNumber, foundWords.Answers)
	msg.Ack(false)
}

func (c *Consumer) Close() error {
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
