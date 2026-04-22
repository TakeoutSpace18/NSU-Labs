package mq

import (
	"context"
	"encoding/json"
	"fmt"
	"sync"
	"time"

	amqp "github.com/rabbitmq/amqp091-go"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
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

type Publisher struct {
	conn    *amqp.Connection
	channel *amqp.Channel
	mu      sync.Mutex
	url     string
	done    chan struct{}
}

func NewPublisher(url string) (*Publisher, error) {
	p := &Publisher{url: url, done: make(chan struct{})}
	if err := p.connect(); err != nil {
		return nil, err
	}
	go p.handleReconnect()
	return p, nil
}

func (p *Publisher) connect() error {
	conn, err := amqp.Dial(p.url)
	if err != nil {
		return fmt.Errorf("amqp dial: %w", err)
	}

	ch, err := conn.Channel()
	if err != nil {
		conn.Close()
		return fmt.Errorf("amqp channel: %w", err)
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

	p.conn = conn
	p.channel = ch
	return nil
}

func (p *Publisher) handleReconnect() {
	for {
		select {
		case <-p.done:
			return
		case err, ok := <-p.conn.NotifyClose(make(chan *amqp.Error, 1)):
			if !ok {
				return
			}
			logger.Log.Warn("RabbitMQ connection lost, reconnecting...", "error", err)
			for {
				select {
				case <-p.done:
					return
				default:
				}
				p.mu.Lock()
				if connectErr := p.connect(); connectErr != nil {
					p.mu.Unlock()
					logger.Log.Error("RabbitMQ reconnect failed, retrying in 5s", "error", connectErr)
					time.Sleep(5 * time.Second)
					continue
				}
				p.mu.Unlock()
				logger.Log.Info("RabbitMQ reconnected successfully")
				break
			}
		}
	}
}

func (p *Publisher) Publish(ctx context.Context, msg FoundWordsMessage) error {
	body, err := json.Marshal(msg)
	if err != nil {
		return fmt.Errorf("marshal message: %w", err)
	}

	p.mu.Lock()
	defer p.mu.Unlock()

	if p.channel == nil {
		return fmt.Errorf("rabbitmq channel not available")
	}

	return p.channel.PublishWithContext(ctx, ExchangeName, RoutingKey, false, false,
		amqp.Publishing{
			ContentType:  "application/json",
			DeliveryMode: amqp.Persistent,
			Body:         body,
		},
	)
}

func (p *Publisher) Close() error {
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
