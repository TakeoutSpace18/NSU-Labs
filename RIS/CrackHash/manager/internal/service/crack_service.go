package service

import (
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/model"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/repository"
	"github.com/google/uuid"
)

type CrackService struct {
	repo *repository.MongoRepository
}

func NewCrackService(repo *repository.MongoRepository) *CrackService {
	return &CrackService{
		repo: repo,
	}
}

func (s *CrackService) CreateRequest(hash string, maxLength int, partCount int, alphabet string) (uuid.UUID, error) {
	id := uuid.New()

	req := &model.CrackRequest{
		ID:                   id.String(),
		Hash:                 hash,
		MaxLength:            maxLength,
		Alphabet:             alphabet,
		Status:               model.StatusInProgress,
		Results:              []string{},
		PartCount:            partCount,
		CompletedParts:       0,
		CompletedPartNumbers: []int32{},
	}

	err := s.repo.Create(req)
	if err != nil {
		return uuid.Nil, err
	}

	logger.Log.Info("Created crack request",
		"id", id,
		"hash", hash,
		"maxLength", maxLength,
		"partCount", partCount,
	)

	return id, nil
}

func (s *CrackService) HandleFoundWords(id uuid.UUID, partNumber int32, answers []string) {
	err := s.repo.AddResults(id, partNumber, answers)
	if err != nil {
		logger.Log.Error("Error adding results to database",
			"requestId", id,
			"partNumber", partNumber,
			"error", err,
		)
		return
	}

	req, err := s.repo.Get(id)
	if err != nil {
		logger.Log.Error("Error reading request from database",
			"requestId", id,
			"error", err,
		)
		return
	}

	if req.CompletedParts >= req.PartCount {
		err := s.repo.MarkReady(id)
		if err != nil {
			logger.Log.Error("Error marking request ready in database",
				"requestId", id,
				"error", err,
			)
		}
	}

	logger.Log.Info("Completed part",
		"requestId", id,
		"part", req.CompletedParts,
		"total", req.PartCount,
	)

	if req.CompletedParts >= req.PartCount {
		logger.Log.Info("All parts completed",
			"requestId", id,
			"results", len(req.Results),
		)
	}
}

func (s *CrackService) GetStatus(id uuid.UUID) (model.StatusResponseStatus, []string, bool) {
	req, err := s.repo.Get(id)
	if err != nil {
		return "", nil, false
	}

	// Only return results if status is READY
	if req.Status != model.StatusReady {
		return req.Status, nil, true
	}

	results := make([]string, len(req.Results))
	copy(results, req.Results)

	return req.Status, results, true
}

func (s *CrackService) SetPendingParts(id uuid.UUID, pendingParts []int32) error {
	return s.repo.SetPendingParts(id, pendingParts)
}
