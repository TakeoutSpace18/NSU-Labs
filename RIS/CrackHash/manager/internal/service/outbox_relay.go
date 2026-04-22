package service

import (
	"context"
	"time"

	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/mq"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/repository"
	"github.com/google/uuid"
)

type OutboxRelay struct {
	repo      *repository.MongoRepository
	publisher *mq.TaskPublisher
	interval  time.Duration
	done      chan struct{}
}

func NewOutboxRelay(
	repo *repository.MongoRepository,
	publisher *mq.TaskPublisher,
	interval time.Duration,
) *OutboxRelay {
	return &OutboxRelay{
		repo:      repo,
		publisher: publisher,
		interval:  interval,
		done:      make(chan struct{}),
	}
}

func (r *OutboxRelay) Start(ctx context.Context) {
	ticker := time.NewTicker(r.interval)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			return
		case <-r.done:
			return
		case <-ticker.C:
			r.processPendingParts(ctx)
		}
	}
}

func (r *OutboxRelay) Close() {
	close(r.done)
}

func (r *OutboxRelay) processPendingParts(ctx context.Context) {
	requests, err := r.repo.FindWithPendingParts()
	if err != nil {
		logger.Log.Error("Error finding requests with pending parts", "error", err)
		return
	}

	for _, req := range requests {
		requestId, parseErr := uuid.Parse(req.ID)
		if parseErr != nil {
			logger.Log.Error("Invalid request ID in pending parts scan", "id", req.ID, "error", parseErr)
			continue
		}

		for _, partNumber := range req.PendingParts {
			msg := mq.CrackTaskMessage{
				RequestId:  req.ID,
				Hash:       req.Hash,
				MaxLength:  int32(req.MaxLength),
				Alphabet:   req.Alphabet,
				PartNumber: partNumber,
				PartCount:  int32(req.PartCount),
			}

			if err := r.publisher.Publish(ctx, msg); err != nil {
				logger.Log.Error("Outbox: failed to publish pending part",
					"requestId", req.ID, "partNumber", partNumber, "error", err)
				break
			}

			logger.Log.Info("Outbox: published pending part",
				"requestId", req.ID, "partNumber", partNumber)

			if err := r.repo.RemovePendingPart(requestId, partNumber); err != nil {
				logger.Log.Error("Outbox: failed to remove pending part from DB",
					"requestId", req.ID, "partNumber", partNumber, "error", err)
			}
		}
	}
}
