package model

type StatusResponseStatus string

const (
	StatusInProgress StatusResponseStatus = "IN_PROGRESS"
	StatusReady      StatusResponseStatus = "READY"
	StatusError      StatusResponseStatus = "ERROR"
)

type CrackRequest struct {
	ID                   string              `bson:"_id"`
	Hash                 string              `bson:"hash"`
	MaxLength            int                 `bson:"max_length"`
	Alphabet             string              `bson:"alphabet"`
	Status               StatusResponseStatus `bson:"status"`
	Results              []string            `bson:"results"`
	PartCount            int                 `bson:"part_count"`
	CompletedParts       int                 `bson:"completed_parts"`
	CompletedPartNumbers []int32             `bson:"completed_part_numbers"`
	PendingParts         []int32             `bson:"pending_parts"`
}