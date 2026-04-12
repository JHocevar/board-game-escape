package main

import (
	"context"
	"errors"
	"fmt"
	"time"

	"github.com/aws/aws-sdk-go-v2/aws"
	"github.com/aws/aws-sdk-go-v2/config"
	"github.com/aws/aws-sdk-go-v2/service/dynamodb"
	"github.com/aws/aws-sdk-go-v2/service/dynamodb/types"
)

func newDynamoDBClient(ctx context.Context) (*dynamodb.Client, error) {
	fmt.Println("Loading default config")
	cfg, err := config.LoadDefaultConfig(ctx,
		config.WithRegion(region),
	)
	if err != nil {
		return nil, fmt.Errorf("failed to load config: %w", err)
	}

	fmt.Println("Creating DynamoDB client")
	client := dynamodb.NewFromConfig(cfg, func(o *dynamodb.Options) {
		o.BaseEndpoint = aws.String(dynamoDbEndpoint)
	})

	fmt.Println("DynamoDB client created successfully")
	return client, nil
}

func tableExists(ctx context.Context, client *dynamodb.Client, name string) (bool, error) {
	fmt.Println("Checking if table exists")
	_, err := client.DescribeTable(ctx, &dynamodb.DescribeTableInput{
		TableName: aws.String(name),
	})
	fmt.Println("DescribeTable call completed")
	if err != nil {
		if _, ok := errors.AsType[*types.ResourceNotFoundException](err); ok {
			return false, nil
		}
		return false, fmt.Errorf("failed to describe table: %w", err)
	}
	return true, nil
}

func createTable(ctx context.Context, client *dynamodb.Client, name string) error {
	_, err := client.CreateTable(ctx, &dynamodb.CreateTableInput{
		TableName: aws.String(name),
		ProvisionedThroughput: &types.ProvisionedThroughput{
			ReadCapacityUnits:  aws.Int64(1000),
			WriteCapacityUnits: aws.Int64(1000),
		},
		AttributeDefinitions: []types.AttributeDefinition{
			{
				AttributeName: aws.String("PK"),
				AttributeType: types.ScalarAttributeTypeS,
			},
			{
				AttributeName: aws.String("SK"),
				AttributeType: types.ScalarAttributeTypeS,
			},
		},
		KeySchema: []types.KeySchemaElement{
			{
				AttributeName: aws.String("PK"),
				KeyType:       types.KeyTypeHash,
			},
			{
				AttributeName: aws.String("SK"),
				KeyType:       types.KeyTypeRange,
			},
		},
	})
	if err != nil {
		return fmt.Errorf("failed to create table: %w", err)
	}

	// Wait until table is active
	waiter := dynamodb.NewTableExistsWaiter(client)
	if err := waiter.Wait(ctx, &dynamodb.DescribeTableInput{
		TableName: aws.String(name),
	}, 30*time.Second); err != nil {
		return fmt.Errorf("failed waiting for table to become active: %w", err)
	}

	fmt.Printf("Table %q created successfully\n", name)
	return nil
}

func ensureTable(ctx context.Context, client *dynamodb.Client, name string) error {
	exists, err := tableExists(ctx, client, name)
	if err != nil {
		return err
	}
	if exists {
		fmt.Printf("Table %q already exists, skipping creation\n", name)
		return nil
	}
	return createTable(ctx, client, name)
}

func resetDynamoDb(ctx context.Context, client *dynamodb.Client, name string) error {
	// Check if table exists before deleting
	exists, err := tableExists(ctx, client, name)
	if err != nil {
		return err
	}
	if !exists {
		fmt.Printf("Table %q does not exist, skipping reset\n", name)
		return nil
	}

	// Delete the table
	_, err = client.DeleteTable(ctx, &dynamodb.DeleteTableInput{
		TableName: aws.String(name),
	})
	if err != nil {
		return fmt.Errorf("failed to delete table: %w", err)
	}

	// Wait until table is deleted
	waiter := dynamodb.NewTableNotExistsWaiter(client)
	if err := waiter.Wait(ctx, &dynamodb.DescribeTableInput{
		TableName: aws.String(name),
	}, 30*time.Second); err != nil {
		return fmt.Errorf("failed waiting for table to be deleted: %w", err)
	}

	fmt.Printf("Table %q deleted successfully\n", name)

	return ensureTable(ctx, client, name)
}
