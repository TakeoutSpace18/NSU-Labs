package main

import (
	"context"
	"flag"
	"os"
	"os/signal"
	"syscall"

	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/mq"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/service"
	"github.com/google/uuid"
)

type Config struct {
	RabbitMQURL string
}

func getEnv(key, fallback string) string {
	if value, exists := os.LookupEnv(key); exists {
		return value
	}
	return fallback
}

func parseFlags() *Config {
	defaultRabbitMQURL := getEnv("RABBITMQ_URL", "amqp://guest:guest@localhost:5672/")

	rabbitMQURL := flag.String("rabbitmq-url", defaultRabbitMQURL, "RabbitMQ connection URL")
	flag.Parse()

	return &Config{
		RabbitMQURL: *rabbitMQURL,
	}
}

func setupServices(rabbitMQURL string) (*mq.Publisher, *service.TaskProcessor, *mq.TaskConsumer, error) {
	publisher, err := mq.NewPublisher(rabbitMQURL)
	if err != nil {
		return nil, nil, nil, err
	}

	taskProcessor := service.NewTaskProcessor()

	taskHandler := func(ctx context.Context, msg mq.CrackTaskMessage) ([]string, error) {
		requestId, err := uuid.Parse(msg.RequestId)
		if err != nil {
			return nil, err
		}

		task := service.CrackTask{
			RequestId:  requestId,
			Hash:       msg.Hash,
			MaxLength:  uint(msg.MaxLength),
			Alphabet:   msg.Alphabet,
			PartNumber: uint32(msg.PartNumber),
			PartCount:  uint32(msg.PartCount),
		}

		results := taskProcessor.ProcessTask(task)
		return service.EnsureSlice(results), nil
	}

	consumer, err := mq.NewTaskConsumer(rabbitMQURL, publisher, taskHandler)
	if err != nil {
		publisher.Close()
		return nil, nil, nil, err
	}

	return publisher, taskProcessor, consumer, nil
}

func main() {
	if err := logger.Init(); err != nil {
		panic(err)
	}

	cfg := parseFlags()

	publisher, _, consumer, err := setupServices(cfg.RabbitMQURL)
	if err != nil {
		logger.Log.Error("Error setting up services", "error", err)
		os.Exit(1)
	}
	defer publisher.Close()
	defer consumer.Close()

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	go consumer.Start(ctx)

	logger.Log.Info("Worker started, consuming tasks from RabbitMQ")

	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)

	select {
	case <-quit:
		logger.Log.Info("Shutdown signal received")
		cancel()
		consumer.Close()
		publisher.Close()
		logger.Log.Info("Worker stopped gracefully")
	}
}
