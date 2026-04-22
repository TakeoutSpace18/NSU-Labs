package service

import (
	"context"
	"fmt"

	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/mq"
	"github.com/google/uuid"
)

type TaskDistributor struct {
	publisher *mq.TaskPublisher
	partCount int
}

func NewTaskDistributor(publisher *mq.TaskPublisher, partCount int) (*TaskDistributor, error) {
	if partCount <= 0 {
		return nil, fmt.Errorf("partCount must be positive, got %d", partCount)
	}
	return &TaskDistributor{
		publisher: publisher,
		partCount: partCount,
	}, nil
}

func (d *TaskDistributor) PartCount() int {
	return d.partCount
}

type DistributeTask struct {
	RequestId uuid.UUID
	Hash      string
	MaxLength int32
	Alphabet  string
}

type DistributeResult struct {
	PendingParts []int32
}

func (d *TaskDistributor) DistributeTask(ctx context.Context, task DistributeTask) DistributeResult {
	logger.Log.Info("Distributing task to workers", "requestId", task.RequestId, "partCount", d.partCount)

	var pending []int32
	for i := 0; i < d.partCount; i++ {
		msg := mq.CrackTaskMessage{
			RequestId:  task.RequestId.String(),
			Hash:       task.Hash,
			MaxLength:  task.MaxLength,
			Alphabet:   task.Alphabet,
			PartNumber: int32(i),
			PartCount:  int32(d.partCount),
		}
		if err := d.publisher.Publish(ctx, msg); err != nil {
			logger.Log.Error("Error publishing task to RabbitMQ", "requestId", task.RequestId, "partNumber", i, "error", err)
			for j := i; j < d.partCount; j++ {
				pending = append(pending, int32(j))
			}
			break
		}
		logger.Log.Info("Published task to RabbitMQ", "requestId", task.RequestId, "partNumber", i)
	}

	return DistributeResult{PendingParts: pending}
}
