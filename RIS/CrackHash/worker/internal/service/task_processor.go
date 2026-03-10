package service

import (
	"context"
	"time"

	managerclient "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/gen/managerclient"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/google/uuid"
)

type TaskProcessor struct {
	managerClient *managerclient.ClientWithResponses
}

func NewTaskProcessor(managerClient *managerclient.ClientWithResponses) *TaskProcessor {
	return &TaskProcessor{
		managerClient: managerClient,
	}
}

type CrackTask struct {
	RequestId  uuid.UUID
	Hash       string
	MaxLength  uint
	Alphabet   string
	PartNumber uint32
	PartCount  uint32
}

func (p *TaskProcessor) ProcessTaskAsync(task CrackTask) {
	go func() {
		results := p.ProcessTask(task)

		// Send results back to manager with timeout
		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()

		resp, err := p.managerClient.HandleFoundWordsWithResponse(
			ctx,
			managerclient.FoundWordsRequest{
				RequestId:  task.RequestId,
				PartNumber: int32(task.PartNumber),
				Answers:    EnsureSlice(results),
			},
		)

		if err != nil {
			logger.Log.Error("Error sending results to manager",
				"requestId", task.RequestId,
				"partNumber", task.PartNumber,
				"error", err,
			)
			return
		}

		if resp.StatusCode() != 200 {
			logger.Log.Warn("Sent results, manager returned non-200 status",
				"requestId", task.RequestId,
				"partNumber", task.PartNumber,
				"statusCode", resp.StatusCode(),
				"body", resp.Body,
			)
		} else {
			logger.Log.Info("Sent results to manager",
				"requestId", task.RequestId,
				"partNumber", task.PartNumber,
			)
		}
	}()
}

func (p *TaskProcessor) ProcessTask(task CrackTask) []string {
	totalWords := TotalWordCount(uint64(task.MaxLength), uint64(len(task.Alphabet)))

	// Part numbers are 0-indexed
	chunkSize := totalWords / uint64(task.PartCount)

	startIndex := chunkSize * uint64(task.PartNumber)
	stopIndex := startIndex + chunkSize

	// Add remainder if it is the last part
	if task.PartNumber == task.PartCount-1 {
		stopIndex = totalWords
	}

	logger.Log.Info("Starting task",
		"requestId", task.RequestId,
		"partNumber", task.PartNumber,
		"partCount", task.PartCount,
		"startIndex", startIndex,
		"stopIndex", stopIndex,
	)

	words := GenWords(startIndex, stopIndex, []rune(task.Alphabet), int(task.MaxLength))
	results := CrackHash(task.Hash, words)

	logger.Log.Info("Task completed",
		"requestId", task.RequestId,
		"partNumber", task.PartNumber,
		"results", results,
	)

	return results
}
