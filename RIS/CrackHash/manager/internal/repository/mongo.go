package repository

import (
	"context"
	"time"

	"github.com/TakeoutSpace18/NSU-Labs/RIS/CrackHash/manager/internal/model"
	"github.com/google/uuid"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/writeconcern"
)

type MongoRepository struct {
	client     *mongo.Client
	collection *mongo.Collection
}

func NewMongoRepository(uri string) (*MongoRepository, error) {
	clientOpts := options.Client().ApplyURI(uri).SetServerSelectionTimeout(30 * time.Second)

	client, err := mongo.Connect(context.Background(), clientOpts)
	if err != nil {
		return nil, err
	}

	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	if err := client.Ping(ctx, nil); err != nil {
		return nil, err
	}

	col := client.Database("crackhash").Collection("requests",
		options.Collection().SetWriteConcern(writeconcern.Majority()),
	)

	return &MongoRepository{client: client, collection: col}, nil
}

func (r *MongoRepository) Close() error {
	return r.client.Disconnect(context.Background())
}

func (r *MongoRepository) Create(req *model.CrackRequest) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	_, err := r.collection.InsertOne(ctx, req)
	return err
}

func (r *MongoRepository) Get(id uuid.UUID) (*model.CrackRequest, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	var req model.CrackRequest
	err := r.collection.FindOne(ctx, bson.M{"_id": id.String()}).Decode(&req)
	if err != nil {
		return nil, err
	}

	return &req, nil
}

func (r *MongoRepository) AddResults(id uuid.UUID, partNumber int32, answers []string) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	filter := bson.M{
		"_id":                    id.String(),
		"completed_part_numbers": bson.M{"$ne": partNumber},
	}

	update := bson.M{
		"$push": bson.M{
			"results": bson.M{
				"$each": answers,
			},
			"completed_part_numbers": partNumber,
		},
		"$inc": bson.M{
			"completed_parts": 1,
		},
	}

	_, err := r.collection.UpdateOne(ctx, filter, update)
	return err
}

func (r *MongoRepository) MarkReady(id uuid.UUID) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	_, err := r.collection.UpdateOne(ctx,
		bson.M{"_id": id.String()},
		bson.M{"$set": bson.M{"status": string(model.StatusReady)}},
	)

	return err
}

func (r *MongoRepository) SetPendingParts(id uuid.UUID, pendingParts []int32) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	_, err := r.collection.UpdateOne(ctx,
		bson.M{"_id": id.String()},
		bson.M{"$set": bson.M{"pending_parts": pendingParts}},
	)
	return err
}

func (r *MongoRepository) RemovePendingPart(id uuid.UUID, partNumber int32) error {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	filter := bson.M{
		"_id":           id.String(),
		"pending_parts": partNumber,
	}
	update := bson.M{"$pull": bson.M{"pending_parts": partNumber}}
	_, err := r.collection.UpdateOne(ctx, filter, update)
	return err
}

func (r *MongoRepository) FindWithPendingParts() ([]*model.CrackRequest, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	filter := bson.M{
		"pending_parts": bson.M{"$exists": true, "$ne": []int32{}},
		"status":        string(model.StatusInProgress),
	}

	cursor, err := r.collection.Find(ctx, filter)
	if err != nil {
		return nil, err
	}
	defer cursor.Close(ctx)

	var results []*model.CrackRequest
	if err := cursor.All(ctx, &results); err != nil {
		return nil, err
	}
	return results, nil
}