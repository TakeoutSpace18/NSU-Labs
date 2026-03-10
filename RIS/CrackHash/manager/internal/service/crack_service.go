package service

import (
	"sync"

	"github.com/google/uuid"
	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/logger"
)

type StatusResponseStatus string

const (
	StatusInProgress StatusResponseStatus = "IN_PROGRESS"
	StatusReady      StatusResponseStatus = "READY"
	StatusError      StatusResponseStatus = "ERROR"
)

type CrackRequest struct {
	ID             uuid.UUID
	Hash           string
	MaxLength      int
	Status         StatusResponseStatus
	Results        []string
	PartCount      int
	CompletedParts int
	mu             sync.RWMutex
}

type CrackService struct {
	requests map[uuid.UUID]*CrackRequest
	mu       sync.RWMutex
}

func NewCrackService() *CrackService {
	return &CrackService{
		requests: make(map[uuid.UUID]*CrackRequest),
	}
}

func (s *CrackService) CreateRequest(hash string, maxLength int, partCount int) uuid.UUID {
	id := uuid.New()

	req := &CrackRequest{
		ID:             id,
		Hash:           hash,
		MaxLength:      maxLength,
		Status:         StatusInProgress,
		Results:        []string{},
		PartCount:      partCount,
		CompletedParts: 0,
	}

	s.mu.Lock()
	s.requests[id] = req
	s.mu.Unlock()

	logger.Log.Info("Created crack request",
		"id", id,
		"hash", hash,
		"maxLength", maxLength,
		"partCount", partCount,
	)

	return id
}

func (s *CrackService) GetRequest(id uuid.UUID) (*CrackRequest, bool) {
	s.mu.RLock()
	req, exists := s.requests[id]
	s.mu.RUnlock()

	return req, exists
}

func (s *CrackService) HandleFoundWords(id uuid.UUID, partNumber int32, answers []string) {
	s.mu.RLock()
	req, exists := s.requests[id]
	s.mu.RUnlock()

	if !exists {
		logger.Log.Warn("Received results for unknown request", "id", id)
		return
	}

	req.mu.Lock()
	defer req.mu.Unlock()

	if len(answers) > 0 {
		req.Results = append(req.Results, answers...)
	}

	req.CompletedParts++

	logger.Log.Info("Completed part",
		"requestId", id,
		"part", req.CompletedParts,
		"total", req.PartCount,
	)

	if req.CompletedParts >= req.PartCount {
		req.Status = StatusReady
		logger.Log.Info("All parts completed",
			"requestId", id,
			"results", len(req.Results),
		)
	}
}

func (s *CrackService) GetStatus(id uuid.UUID) (StatusResponseStatus, []string, bool) {
	req, exists := s.GetRequest(id)
	if !exists {
		return "", nil, false
	}

	req.mu.RLock()
	defer req.mu.RUnlock()

	// Only return results if status is READY
	if req.Status != StatusReady {
		return req.Status, nil, true
	}

	results := make([]string, len(req.Results))
	copy(results, req.Results)

	return req.Status, results, true
}
