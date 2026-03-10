package logger

import (
	"log/slog"
	"os"
)

var Log *slog.Logger

func Init() error {
	handler := slog.NewTextHandler(os.Stdout, &slog.HandlerOptions{
		Level: slog.LevelDebug,
	})
	Log = slog.New(handler)
	slog.SetDefault(Log)
	return nil
}
