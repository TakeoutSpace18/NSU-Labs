package mq

import (
	"context"
	"encoding/json"
	"fmt"
	"sync"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
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

type TaskPublisher struct {
	conn    *amqp.Connection
	channel *amqp.Channel
	confirms chan amqp.Confirmation
	mu      sync.Mutex
	url     string
	done    chan struct{}
}

func NewTaskPublisher(url string) (*TaskPublisher, error) {
	p := &TaskPublisher{url: url, done: make(chan struct{})}
	if err := p.connect(); err != nil {
		logger.Log.Warn("RabbitMQ not available on startup, will retry in background", "error", err)
	}
	go p.handleReconnect()
	return p, nil
}

func (p *TaskPublisher) connect() error {
	conn, err := amqp.Dial(p.url)
	if err != nil {
		return fmt.Errorf("amqp dial: %w", err)
	}

	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return fmt.Errorf("amqp channel: %w", err)
	}

	if err := ch.Confirm(false); err != nil {
		ch.Close()
		conn.Close()
		return fmt.Errorf("channel confirm: %w", err)
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

	p.confirms = ch.NotifyPublish(make(chan amqp.Confirmation, 1))
	p.conn = conn
	p.channel = ch
	return nil
}

func (p *TaskPublisher) handleReconnect() {
	for {
		p.mu.Lock()
		conn := p.conn
		p.mu.Unlock()

		if conn == nil {
			p.mu.Lock()
			if err := p.connect(); err != nil {
				p.mu.Unlock()
				logger.Log.Error("RabbitMQ connect failed, retrying in 5s", "error", err)
				select {
				case <-p.done:
					return
				case <-time.After(5 * time.Second):
					continue
				}
			}
			p.mu.Unlock()
			logger.Log.Info("RabbitMQ connected successfully")
		}

		p.mu.Lock()
		conn = p.conn
		p.mu.Unlock()

		select {
		case <-p.done:
			return
		case err, ok := <-conn.NotifyClose(make(chan *amqp.Error, 1)):
			if !ok {
				return
			}
			logger.Log.Warn("RabbitMQ connection lost, reconnecting...", "error", err)
			p.mu.Lock()
			p.conn = nil
			p.channel = nil
			p.confirms = nil
			p.mu.Unlock()
		}
	}
}

func (p *TaskPublisher) Publish(ctx context.Context, msg CrackTaskMessage) error {
	body, err := json.Marshal(msg)
	if err != nil {
		return fmt.Errorf("marshal message: %w", err)
	}

	p.mu.Lock()
	defer p.mu.Unlock()

	if p.channel == nil {
		return fmt.Errorf("rabbitmq channel not available")
	}

	confirms := p.confirms

	if err := p.channel.PublishWithContext(ctx, TaskExchangeName, TaskRoutingKey, false, false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		},
	); err != nil {
		return fmt.Errorf("publish: %w", err)
	}

	select {
	case confirm, ok := <-confirms:
		if !ok {
			return fmt.Errorf("confirm channel closed")
		}
		if !confirm.Ack {
			return fmt.Errorf("message nacked by broker")
		}
		return nil
	case <-ctx.Done():
		return fmt.Errorf("confirm wait cancelled: %w", ctx.Err())
	}
}

func (p *TaskPublisher) Close() error {
	close(p.done)
	p.mu.Lock()
	defer p.mu.Unlock()
	if p.channel != nil {
		p.channel.Close()
	}
	if p.conn != nil {
		return p.conn.Close()
	}
	return nil
}
