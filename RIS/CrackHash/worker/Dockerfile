FROM golang:1.26-alpine AS builder

WORKDIR /app

COPY api/ ./api/
COPY worker/ ./worker/

WORKDIR /app/worker
RUN go mod download
RUN go generate -x ./internal/gen/generate.go
RUN CGO_ENABLED=0 go build -o worker ./cmd

FROM alpine:latest

WORKDIR /root/

COPY --from=builder /app/worker/worker .

EXPOSE 8081

CMD ["./worker"]
