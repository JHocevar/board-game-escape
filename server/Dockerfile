FROM golang:1.26-alpine AS builder

WORKDIR /src

COPY go.mod .
COPY main.go .

RUN go build -o /app/main ./...

FROM alpine:3.19

RUN apk add --no-cache ca-certificates
COPY --from=builder /app/main /app/main

EXPOSE 8080

ENTRYPOINT ["/app/main"]
