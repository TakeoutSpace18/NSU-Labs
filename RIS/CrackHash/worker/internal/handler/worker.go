package handler

import (
	"context"

	srv "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/gen/server"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/worker/internal/service"
)

// Worker implements StrictServerInterface
type Worker struct {
	taskProcessor *service.TaskProcessor
}

func NewWorker(taskProcessor *service.TaskProcessor) *Worker {
	return &Worker{
		taskProcessor: taskProcessor,
	}
}

// (POST /internal/api/worker/hash/crack/task)
func (w *Worker) SubmitTask(
	ctx context.Context,
	request srv.SubmitTaskRequestObject,
) (srv.SubmitTaskResponseObject, error) {
	maxLen := uint64(request.Body.MaxLength)
	alphaLen := uint64(len(request.Body.Alphabet))
	totalWords := service.TotalWordCount(maxLen, alphaLen)

	logger.Log.Info("totalWords",
		"totalWords", totalWords,
		"MaxLength", request.Body.MaxLength,
		"Alphabet", len(request.Body.Alphabet),
	)

	if uint64(request.Body.PartCount) > totalWords {
		logger.Log.Warn("Invalid request: PartCount exceeds total words",
			"partCount", request.Body.PartCount,
			"totalWords", totalWords,
		)
		return srv.SubmitTask400JSONResponse{
			Error:   "INVALID_REQUEST",
			Message: "PartCount exceeds total number of words",
		}, nil
	}

	task := service.CrackTask{
		RequestId:  request.Body.RequestId,
		Hash:       request.Body.Hash,
		MaxLength:  uint(request.Body.MaxLength),
		Alphabet:   request.Body.Alphabet,
		PartNumber: uint32(request.Body.PartNumber),
		PartCount:  uint32(request.Body.PartCount),
	}

	w.taskProcessor.ProcessTaskAsync(task)

	return srv.SubmitTask200Response{}, nil
}
