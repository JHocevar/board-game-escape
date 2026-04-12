package main

import (
	"fmt"
	"net/http"

	"github.com/aws/aws-sdk-go-v2/service/dynamodb"
)

func rootHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, "hi gamers, how we doin?\n")
}

func resetTableHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if err := resetDynamoDb(r.Context(), client, tableName); err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		fmt.Fprint(w, "table reset complete\n")
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
		fmt.Fprintf(w, "tag '%s' saved successfully\n", tag.Id)
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
		fmt.Fprintf(w, "tag '%s' deleted successfully\n", tagId)
	}
}

func listTagsHandler(client *dynamodb.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		tags, err := listTags(r.Context(), client)
		if err != nil {
			http.Error(w, err.Error(), http.StatusInternalServerError)
			return
		}
		for _, tag := range tags {
			fmt.Fprintf(w, "%s: %s\n", tag.Id, tag.Content)
		}
	}
}
