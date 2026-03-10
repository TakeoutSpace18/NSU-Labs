package handler

import (
	"context"

	srv "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/gen/server"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/service"
)

// Manager implements StrictServerInterface
type Manager struct {
	crackService      *service.CrackService
	workerDistributor *service.WorkerDistributor
}

func NewManager(
	crackService *service.CrackService,
	workerDistributor *service.WorkerDistributor,
) *Manager {
	return &Manager{
		crackService:      crackService,
		workerDistributor: workerDistributor,
	}
}

// POST /api/hash/crack
func (m *Manager) CrackHash(
	ctx context.Context,
	request srv.CrackHashRequestObject,
) (srv.CrackHashResponseObject, error) {
	workerCount := m.workerDistributor.WorkerCount()

	requestId := m.crackService.CreateRequest(
		request.Body.Hash,
		request.Body.MaxLength,
		workerCount,
	)

	logger.Log.Info("Created crack request",
		"requestId", requestId,
		"hash", request.Body.Hash,
		"maxLength", request.Body.MaxLength,
		"workerCount", workerCount,
	)

	alphabet := "abcdefghijklmnopqrstuvwxyz0123456789"

	err := m.workerDistributor.DistributeTask(ctx, service.DistributeTask{
		RequestId: requestId,
		Hash:      request.Body.Hash,
		MaxLength: int32(request.Body.MaxLength),
		Alphabet:  alphabet,
	})

	if err != nil {
		logger.Log.Error("Error distributing task", "error", err)
		return srv.CrackHash400JSONResponse{
			Error:   "DISTRIBUTION_ERROR",
			Message: "Failed to distribute task to workers",
		}, nil
	}

	return srv.CrackHash200JSONResponse{
		RequestId: requestId,
	}, nil
}

// GET /api/hash/status
func (m *Manager) GetCrackStatus(
	ctx context.Context,
	request srv.GetCrackStatusRequestObject,
) (srv.GetCrackStatusResponseObject, error) {
	status, results, exists := m.crackService.GetStatus(request.Params.RequestId)

	if !exists {
		return srv.GetCrackStatus404JSONResponse{
			Error:   "NOT_FOUND",
			Message: "Request not found",
		}, nil
	}

	var data *[]string
	if status == service.StatusReady {
		data = &results
	}

	return srv.GetCrackStatus200JSONResponse{
		Status: srv.StatusResponseStatus(status),
		Data:   data,
	}, nil
}

// PATCH /internal/api/manager/hash/crack/request
func (m *Manager) HandleFoundWords(
	ctx context.Context,
	request srv.HandleFoundWordsRequestObject,
) (srv.HandleFoundWordsResponseObject, error) {
	logger.Log.Info("Received results from worker",
		"requestId", request.Body.RequestId,
		"partNumber", request.Body.PartNumber,
		"answers", len(request.Body.Answers),
	)

	m.crackService.HandleFoundWords(
		request.Body.RequestId,
		request.Body.PartNumber,
		request.Body.Answers,
	)

	return srv.HandleFoundWords200Response{}, nil
}
