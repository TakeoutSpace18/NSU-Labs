package main

import (
	"context"
	"flag"
	"net"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"syscall"
	"time"

	"github.com/getkin/kin-openapi/openapi3"
	"github.com/go-chi/chi/v5"
	chimiddleware "github.com/go-chi/chi/v5/middleware"

	oapimiddleware "github.com/oapi-codegen/nethttp-middleware"

	"github.com/rs/cors"

	srv "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/gen/server"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/handler"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/mq"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/repository"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/service"
)

type Config struct {
	Port        string
	PartCount   int
	MongoURI    string
	RabbitMQURL string
}

func getEnv(key, fallback string) string {
	if value, exists := os.LookupEnv(key); exists {
		return value
	}
	return fallback
}

func getEnvInt(key string, fallback int) int {
	if value, exists := os.LookupEnv(key); exists {
		if intVal, err := strconv.Atoi(value); err == nil {
			return intVal
		}
	}
	return fallback
}

func parseFlags() *Config {
	defaultPort := getEnv("PORT", "8080")
	defaultPartCount := getEnvInt("PART_COUNT", 5)
	defaultMongoURI := getEnv("MONGO_URI", "mongodb://localhost:27017")
	defaultRabbitMQURL := getEnv("RABBITMQ_URL", "amqp://guest:guest@localhost:5672/")

	port := flag.String("port", defaultPort, "Port for manager service")
	partCount := flag.Int("part-count", defaultPartCount, "Number of parts to split crack tasks into")
	mongoURI := flag.String("mongo-uri", defaultMongoURI, "MongoDB connection URI")
	rabbitMQURL := flag.String("rabbitmq-url", defaultRabbitMQURL, "RabbitMQ connection URL")
	flag.Parse()

	logger.Log.Info("Manager configuration",
		"port", *port,
		"partCount", *partCount)

	return &Config{
		Port:        *port,
		PartCount:   *partCount,
		MongoURI:    *mongoURI,
		RabbitMQURL: *rabbitMQURL,
	}
}

func setupRouter(swaggerSpec *openapi3.T, handler srv.ServerInterface) *chi.Mux {
	rootRouter := chi.NewRouter()
	rootRouter.Use(chimiddleware.RequestID)
	rootRouter.Use(chimiddleware.Logger)
	rootRouter.Use(chimiddleware.Recoverer)
	rootRouter.Use(cors.AllowAll().Handler)

	apiRouter := chi.NewRouter()
	apiRouter.Use(oapimiddleware.OapiRequestValidator(swaggerSpec))

	srv.HandlerFromMux(handler, apiRouter)
	rootRouter.Mount("/", apiRouter)

	return rootRouter
}

func setupServices(partCount int, mongoURI string, rabbitMQURL string) (*handler.Manager, *mq.Consumer, *mq.TaskPublisher, *service.OutboxRelay, error) {
	repo, err := repository.NewMongoRepository(mongoURI)
	if err != nil {
		return nil, nil, nil, nil, err
	}

	crackService := service.NewCrackService(repo)

	taskPublisher, err := mq.NewTaskPublisher(rabbitMQURL)
	if err != nil {
		return nil, nil, nil, nil, err
	}

	taskDistributor, err := service.NewTaskDistributor(taskPublisher, partCount)
	if err != nil {
		taskPublisher.Close()
		return nil, nil, nil, nil, err
	}

	outboxRelay := service.NewOutboxRelay(repo, taskPublisher, 10*time.Second)

	consumer, err := mq.NewConsumer(rabbitMQURL, crackService.HandleFoundWords)
	if err != nil {
		taskPublisher.Close()
		return nil, nil, nil, nil, err
	}

	manager := handler.NewManager(crackService, taskDistributor)
	return manager, consumer, taskPublisher, outboxRelay, nil
}

func main() {
	if err := logger.Init(); err != nil {
		panic(err)
	}

	cfg := parseFlags()

	swaggerSpec, err := srv.GetSwagger()
	if err != nil {
		logger.Log.Error("Error loading swagger spec", "error", err)
		os.Exit(1)
	}
	swaggerSpec.Servers = nil

	managerHandler, consumer, taskPublisher, outboxRelay, err := setupServices(cfg.PartCount, cfg.MongoURI, cfg.RabbitMQURL)
	if err != nil {
		logger.Log.Error("Error setting up services", "error", err)
		os.Exit(1)
	}
	defer taskPublisher.Close()
	defer outboxRelay.Close()

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()
	go consumer.Start(ctx)
	go outboxRelay.Start(ctx)

	strictHandler := srv.NewStrictHandler(managerHandler, nil)
	router := setupRouter(swaggerSpec, strictHandler)

	router.Get("/openapi.json", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		data, _ := swaggerSpec.MarshalJSON()
		w.Write(data)
	})

	server := &http.Server{
		Handler: router,
		Addr:    net.JoinHostPort("0.0.0.0", cfg.Port),
	}

	serverErrors := make(chan error, 1)
	go func() {
		logger.Log.Info("Manager started", "addr", server.Addr)
		serverErrors <- server.ListenAndServe()
	}()

	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)

	select {
	case <-quit:
		logger.Log.Info("Shutdown signal received")

		cancel()
		consumer.Close()

		shutdownCtx, shutdownCancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer shutdownCancel()

		if err := server.Shutdown(shutdownCtx); err != nil {
			logger.Log.Error("Forced shutdown", "error", err)
			os.Exit(1)
		}
		logger.Log.Info("Server stopped gracefully")

	case err := <-serverErrors:
		logger.Log.Error("Server error", "error", err)
		os.Exit(1)
	}
}
