package main

import (
	"context"
	"flag"
	"net"
	"net/http"
	"os"
	"os/signal"
	"strings"
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
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/service"
)

type Config struct {
	Port       string
	WorkerURLs []string
}

func getEnv(key, fallback string) string {
	if value, exists := os.LookupEnv(key); exists {
		return value
	}
	return fallback
}

func parseFlags() *Config {
	defaultPort := getEnv("PORT", "8080")
	defaultWorkerURLs := getEnv("WORKER_URL", "http://localhost:8081")

	port := flag.String("port", defaultPort, "Port for manager service")
	workerURLs := flag.String("worker-urls", defaultWorkerURLs, "Comma-separated list of worker URLs")
	flag.Parse()

	urlList := strings.Split(*workerURLs, ",")
	for i, url := range urlList {
		urlList[i] = strings.TrimSpace(url)
	}

	logger.Log.Info("Manager configuration",
		"port", *port,
		"workerUrls", urlList)

	return &Config{
		Port:       *port,
		WorkerURLs: urlList,
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

func setupServices(workerURLs []string) (*handler.Manager, error) {
	workerDistributor, err := service.NewWorkerDistributor(workerURLs)
	if err != nil {
		return nil, err
	}

	crackService := service.NewCrackService()
	manager := handler.NewManager(crackService, workerDistributor)

	return manager, nil
}

func runServer(server *http.Server) error {
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
		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()

		if err := server.Shutdown(ctx); err != nil {
			logger.Log.Error("Forced shutdown", "error", err)
			return err
		}
		logger.Log.Info("Server stopped gracefully")
		return nil

	case err := <-serverErrors:
		logger.Log.Error("Server error", "error", err)
		return err
	}
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

	manager, err := setupServices(cfg.WorkerURLs)
	if err != nil {
		logger.Log.Error("Error setting up services", "error", err)
		os.Exit(1)
	}

	managerHandler := srv.NewStrictHandler(manager, nil)

	router := setupRouter(swaggerSpec, managerHandler)

	router.Get("/openapi.json", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		data, _ := swaggerSpec.MarshalJSON()
		w.Write(data)
	})

	server := &http.Server{
		Handler: router,
		Addr:    net.JoinHostPort("0.0.0.0", cfg.Port),
	}

	if err := runServer(server); err != nil {
		os.Exit(1)
	}
}
