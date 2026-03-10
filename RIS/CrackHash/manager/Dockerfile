FROM golang:1.26-alpine AS builder

WORKDIR /app

COPY api/ ./api/
COPY manager/ ./manager/

WORKDIR /app/manager
RUN go mod download
RUN go generate -x ./internal/gen/generate.go
RUN CGO_ENABLED=0 go build -o manager ./cmd

FROM alpine:latest

WORKDIR /root/

COPY --from=builder /app/manager/manager .

EXPOSE 8080

CMD ["./manager"]
