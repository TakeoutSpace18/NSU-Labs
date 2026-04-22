package handler

import (
	"context"

	srv "github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/gen/server"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/model"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/service"
)

// Manager implements StrictServerInterface
type Manager struct {
	crackService    *service.CrackService
	taskDistributor *service.TaskDistributor
}

func NewManager(
	crackService *service.CrackService,
	taskDistributor *service.TaskDistributor,
) *Manager {
	return &Manager{
		crackService:    crackService,
		taskDistributor: taskDistributor,
	}
}

// POST /api/hash/crack
func (m *Manager) CrackHash(
	ctx context.Context,
	request srv.CrackHashRequestObject,
) (srv.CrackHashResponseObject, error) {
	partCount := m.taskDistributor.PartCount()
	alphabet := "abcdefghijklmnopqrstuvwxyz0123456789"

	requestId, err := m.crackService.CreateRequest(
		request.Body.Hash,
		request.Body.MaxLength,
		partCount,
		alphabet,
	)

	if err != nil {
		logger.Log.Error("Error creating crack request", "error", err)
		return srv.CrackHash400JSONResponse{
			Error:   "INTERNAL_ERROR",
			Message: "Failed to create crack request",
		}, nil
	}

	logger.Log.Info("Created crack request",
		"requestId", requestId,
		"hash", request.Body.Hash,
		"maxLength", request.Body.MaxLength,
		"partCount", partCount,
	)

	result := m.taskDistributor.DistributeTask(ctx, service.DistributeTask{
		RequestId: requestId,
		Hash:      request.Body.Hash,
		MaxLength: int32(request.Body.MaxLength),
		Alphabet:  alphabet,
	})

	if err := m.crackService.SetPendingParts(requestId, result.PendingParts); err != nil {
		logger.Log.Error("Error saving pending parts",
			"requestId", requestId, "error", err)
	}

	if len(result.PendingParts) > 0 {
		logger.Log.Warn("Some parts could not be published, queued for retry",
			"requestId", requestId,
			"pendingParts", result.PendingParts,
		)
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
	if status == model.StatusReady {
		data = &results
	}

	return srv.GetCrackStatus200JSONResponse{
		Status: srv.StatusResponseStatus(status),
		Data:   data,
	}, nil
}
