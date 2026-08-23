package main

import (
	"context"
	"fmt"
	"net/http"
	"path/filepath"
	"runtime"

	"github.com/aws/aws-sdk-go-v2/service/dynamodb"
)

func initDynamoDb(ctx context.Context, tableName string) (*dynamodb.Client, error) {
	fmt.Printf("initializing DynamoDB client and ensuring table '%s' exists...\n", tableName)
	client, err := newDynamoDBClient(ctx)
	if err != nil {
		return nil, fmt.Errorf("failed to create DynamoDB client: %w", err)
	}

	if err := ensureTable(ctx, client, tableName); err != nil {
		return nil, fmt.Errorf("failed to ensure table exists: %w", err)
	}

	return client, nil
}

func resolveFrontendBuildDir() string {
	_, currentFile, _, ok := runtime.Caller(0)
	if !ok {
		return filepath.Join("..", "..", "frontend", "build")
	}
	return filepath.Join(filepath.Dir(currentFile), "..", "..", "frontend", "build")
}

func startServer(client *dynamodb.Client) {
	frontendBuildDir := resolveFrontendBuildDir()

	http.HandleFunc("GET /", rootHandler)
	http.HandleFunc("GET /tag", getTagHandler(client))
	http.HandleFunc("POST /reset", resetTableHandler(client))
	http.HandleFunc("POST /tag", saveTagHandler(client))
	http.HandleFunc("DELETE /tag", deleteTagHandler(client))
	http.HandleFunc("GET /tags", listTagsHandler(client))
	http.HandleFunc("GET /api/tags", listTagsHandler(client))
	http.HandleFunc("GET /app", func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, "/app/", http.StatusMovedPermanently)
	})
	http.Handle("GET /app/", http.StripPrefix("/app/", http.FileServer(http.Dir(frontendBuildDir))))

	println("listening on :8080")

	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		panic(err)
	}
}

func main() {
	ctx := context.Background()

	// Initialize DynamoDb and ensure table exists
	dbClient, err := initDynamoDb(ctx, tableName)
	if err != nil {
		panic(fmt.Errorf("failed to initialize DynamoDB: %w", err))
	}

	// Start the HTTP server
	startServer(dbClient)
}
