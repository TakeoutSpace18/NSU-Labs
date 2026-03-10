package main

import (
	"context"
	"flag"
	"net"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/getkin/kin-openapi/openapi3"
	"github.com/go-chi/chi/v5"
	chimiddleware "github.com/go-chi/chi/v5/middleware"

	oapimiddleware "github.com/oapi-codegen/nethttp-middleware"

	"github.com/rs/cors"

	managerclient "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/gen/managerclient"
	srv "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/gen/server"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/handler"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/service"
)

type Config struct {
	Port       string
	ManagerURL string
}

func getEnv(key, fallback string) string {
	if value, exists := os.LookupEnv(key); exists {
		return value
	}
	return fallback
}

func parseFlags() *Config {
	defaultPort := getEnv("PORT", "8081")
	defaultManagerURL := getEnv("MANAGER_URL", "http://localhost:8080")

	port := flag.String("port", defaultPort, "Port for worker service")
	managerURL := flag.String("manager-url", defaultManagerURL, "Manager service URL")
	flag.Parse()

	return &Config{
		Port:       *port,
		ManagerURL: *managerURL,
	}
}

func setupRouter(swaggerSpec *openapi3.T, handler srv.ServerInterface) *chi.Mux {
	rootRouter := chi.NewRouter()
	rootRouter.Use(chimiddleware.RequestID)
	rootRouter.Use(chimiddleware.Logger)
	rootRouter.Use(chimiddleware.Recoverer)
	rootRouter.Use(cors.AllowAll().Handler)

	rootRouter.Get("/openapi.json", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		data, err := swaggerSpec.MarshalJSON()
		if err != nil {
			logger.Log.Error("Error marshaling swagger JSON", "error", err)
			http.Error(w, "Internal Server Error", http.StatusInternalServerError)
			return
		}
		w.Write(data)
	})

	apiRouter := chi.NewRouter()
	apiRouter.Use(oapimiddleware.OapiRequestValidator(swaggerSpec))

	srv.HandlerFromMux(handler, apiRouter)
	rootRouter.Mount("/", apiRouter)

	return rootRouter
}

func setupServices(managerURL string) (*handler.Worker, error) {
	managerClient, err := managerclient.NewClientWithResponses(managerURL)
	if err != nil {
		return nil, err
	}

	taskProcessor := service.NewTaskProcessor(managerClient)
	worker := handler.NewWorker(taskProcessor)

	return worker, nil
}

func runServer(server *http.Server) error {
	serverErrors := make(chan error, 1)

	go func() {
		logger.Log.Info("Worker started", "addr", server.Addr)
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

	worker, err := setupServices(cfg.ManagerURL)
	if err != nil {
		logger.Log.Error("Error setting up services", "error", err)
		os.Exit(1)
	}

	workerHandler := srv.NewStrictHandler(worker, nil)

	router := setupRouter(swaggerSpec, workerHandler)

	server := &http.Server{
		Handler: router,
		Addr:    net.JoinHostPort("0.0.0.0", cfg.Port),
	}

	if err := runServer(server); err != nil {
		os.Exit(1)
	}
}
