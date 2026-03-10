package service

import (
	"context"
	"log"
	"time"

	workerclient "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/gen/workerclient"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/google/uuid"
)

type WorkerDistributor struct {
	workerCount int
	workerURLs  []string
	clients     []*workerclient.ClientWithResponses
}

func NewWorkerDistributor(workerURLs []string) (*WorkerDistributor, error) {
	if len(workerURLs) == 0 {
		log.Fatalln("Zero worker urls passed")
	}

	clients := make([]*workerclient.ClientWithResponses, len(workerURLs))

	for i, url := range workerURLs {
		client, err := workerclient.NewClientWithResponses(url)
		if err != nil {
			return nil, err
		}
		clients[i] = client
	}

	return &WorkerDistributor{
		workerCount: len(workerURLs),
		workerURLs:  workerURLs,
		clients:     clients,
	}, nil
}

func (d *WorkerDistributor) WorkerCount() int {
	return d.workerCount
}

type DistributeTask struct {
	RequestId uuid.UUID
	Hash      string
	MaxLength int32
	Alphabet  string
}

func (d *WorkerDistributor) DistributeTask(ctx context.Context, task DistributeTask) error {
	logger.Log.Info("Distributing task to workers",
		"requestId", task.RequestId,
		"workerCount", d.workerCount,
	)

	for i := 0; i < d.workerCount; i++ {
		go func(partNumber int) {
			workerCtx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
			defer cancel()

			workerTask := workerclient.SubmitTaskJSONRequestBody{
				RequestId:  task.RequestId,
				Hash:       task.Hash,
				MaxLength:  task.MaxLength,
				Alphabet:   task.Alphabet,
				PartNumber: int32(partNumber),
				PartCount:  int32(d.workerCount),
			}

			resp, err := d.clients[partNumber].SubmitTaskWithResponse(workerCtx, workerTask)
			if err != nil {
				logger.Log.Error("Error sumbitting task to worker",
					"requestId", task.RequestId,
					"partNumber", partNumber,
					"error", err,
				)
				return
			}

			if resp.StatusCode() != 200 {
				logger.Log.Warn("Submitted task, worker returned non-200 status",
					"requestId", task.RequestId,
					"partNumber", partNumber,
					"statusCode", resp.StatusCode(),
					"body", resp.Body,
				)
				return
			}

			logger.Log.Info("Submitted task to worker",
				"requestId", task.RequestId,
				"partNumber", partNumber,
			)
		}(i)
	}

	return nil
}
