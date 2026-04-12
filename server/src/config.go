package main

import (
	"os"
)

const (
	// Database
	tableName = "board-game-escape"
	region    = "fakeRegion"

	// Credentials (faked)
	accessKeyID     = "fake-access-key-id"
	secretAccessKey = "fake-secret-access-key"
	sessionToken    = "" // Needs to be empty
)

var (
	// Database
	dynamoDbEndpoint = getEnvOrDefault("DYNAMODB_ENDPOINT", "http://192.168.4.51:8000")
)

func getEnvOrDefault(envKey, defaultValue string) string {
	if value := os.Getenv(envKey); value != "" {
		return value
	}
	return defaultValue
}
