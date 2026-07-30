package main

import (
	"encoding/json"
	"fmt"
	"net/http"

	"github.com/aws/aws-sdk-go-v2/service/dynamodb"
)

type jsonResponse struct {
	Message string `json:"message"`
}

func rootHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(jsonResponse{Message: "hi gamers, how we doin?"})
}

func resetTableHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if err := resetDynamoDb(r.Context(), client, tableName); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(jsonResponse{Message: "table reset complete"})
	}
}

func saveTagHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		// Read tag and content from query parameters
		tagId := r.URL.Query().Get("id")
		content := r.URL.Query().Get("content")

		if tagId == "" || content == "" {
			http.Error(w, "missing required parameters", http.StatusBadRequest)
			return
		}

		// Save tag
		tag := Tag{Id: tagId, Content: content}
		if err := saveTag(r.Context(), client, tag); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(jsonResponse{Message: fmt.Sprintf("tag '%s' saved successfully", tag.Id)})
	}
}

func deleteTagHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

		// Read tag from query parameters
		tagId := r.URL.Query().Get("id")
		if tagId == "" {
			http.Error(w, "missing required parameter", http.StatusBadRequest)
			return
		}

		// Delete tag
		if err := deleteTag(r.Context(), client, tagId); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(jsonResponse{Message: fmt.Sprintf("tag '%s' deleted successfully", tagId)})
	}
}

func listTagsHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		tags, err := listTags(r.Context(), client)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		if err := json.NewEncoder(w).Encode(tags); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
		}
	}
}

func getTagHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		id := r.URL.Query().Get("id")
		tag, err := getTag(r.Context(), client, id)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		if tag != nil {
			if err := json.NewEncoder(w).Encode(tag); err != nil {
				http.Error(w, err.Error(), http.StatusInternalServerError)
			}
		} else {
			http.NotFound(w, r)
		}
	}
}
