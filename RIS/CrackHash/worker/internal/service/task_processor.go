package service

import (
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/google/uuid"
)

type TaskProcessor struct{}

func NewTaskProcessor() *TaskProcessor {
	return &TaskProcessor{}
}

type CrackTask struct {
	RequestId  uuid.UUID
	Hash       string
	MaxLength  uint
	Alphabet   string
	PartNumber uint32
	PartCount  uint32
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
