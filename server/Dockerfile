FROM golang:1.26-alpine AS builder

# Build inside the server subdirectory so go.mod is available
WORKDIR /src/server

# Copy only the server subdirectory into /src/server so go.mod is present
COPY server/ ./

RUN go mod download && go build -o /app/main ./...

FROM alpine:3.19

RUN apk add --no-cache ca-certificates
COPY --from=builder /app/main /app/main
# Copy frontend build assets into the final image so the server can serve them
COPY frontend/build /frontend/build
COPY frontend/build /src/frontend/build

EXPOSE 8080

ENTRYPOINT ["/app/main"]
