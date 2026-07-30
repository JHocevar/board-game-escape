package main

import (
	"context"
	"fmt"

	"github.com/aws/aws-sdk-go-v2/aws"
	"github.com/aws/aws-sdk-go-v2/service/dynamodb"
	"github.com/aws/aws-sdk-go-v2/service/dynamodb/types"
)

type Tag struct {
	Id      string
	Content string
}

func saveTag(ctx context.Context, client *dynamodb.Client, tag Tag) error {
	_, err := client.PutItem(ctx, &dynamodb.PutItemInput{
		TableName: aws.String(tableName),
		Item: map[string]types.AttributeValue{
			"PK":      &types.AttributeValueMemberS{Value: tag.Id},
			"SK":      &types.AttributeValueMemberS{Value: tag.Id},
			"Content": &types.AttributeValueMemberS{Value: tag.Content},
		},
	})
	if err != nil {
		return fmt.Errorf("failed to save tag %q: %w", tag, err)
	}
	return nil
}

func deleteTag(ctx context.Context, client *dynamodb.Client, tagId string) error {
	_, err := client.DeleteItem(ctx, &dynamodb.DeleteItemInput{
		TableName: aws.String(tableName),
		Key: map[string]types.AttributeValue{
			"PK": &types.AttributeValueMemberS{Value: tagId},
			"SK": &types.AttributeValueMemberS{Value: tagId},
		},
	})
	if err != nil {
		return fmt.Errorf("failed to delete tag with id %q: %w", tagId, err)
	}
	return nil
}

func listTags(ctx context.Context, client *dynamodb.Client) ([]Tag, error) {
	out, err := client.Scan(ctx, &dynamodb.ScanInput{
		TableName: aws.String(tableName),
	})
	if err != nil {
		return nil, fmt.Errorf("failed to scan table: %w", err)
	}

	tags := make([]Tag, 0, len(out.Items))
	for _, item := range out.Items {
		tag := Tag{
			Id:      item["PK"].(*types.AttributeValueMemberS).Value,
			Content: item["Content"].(*types.AttributeValueMemberS).Value,
		}
		tags = append(tags, tag)
	}
	return tags, nil
}

func getTag(ctx context.Context, client *dynamodb.Client, tagId string) (*Tag, error) {
	out, err := client.GetItem(ctx, &dynamodb.GetItemInput{
		TableName: aws.String(tableName),
		Key: map[string]types.AttributeValue{
			"PK": &types.AttributeValueMemberS{Value: tagId},
			"SK": &types.AttributeValueMemberS{Value: tagId},
		},
	})
	if err != nil {
		return nil, fmt.Errorf("failed to get tag with id %q: %w", tagId, err)
	}
	if out.Item == nil {
		return nil, nil // Tag not found
	}

	tag := &Tag{
		Id:      out.Item["PK"].(*types.AttributeValueMemberS).Value,
		Content: out.Item["Content"].(*types.AttributeValueMemberS).Value,
	}
	return tag, nil
}
