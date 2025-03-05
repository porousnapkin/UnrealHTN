// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTaskStatus.generated.h"

// Declare log category for HTN task operations
DECLARE_LOG_CATEGORY_EXTERN(LogHTNTask, Log, All);

/**
 * Enumeration for task execution status.
 * Represents the current state of a task during execution.
 */
UENUM(BlueprintType)
enum class EHTNTaskStatus : uint8
{
	/** Task is currently being executed */
	InProgress UMETA(DisplayName = "In Progress"),
	
	/** Task has completed successfully */
	Succeeded UMETA(DisplayName = "Succeeded"),
	
	/** Task has failed during execution */
	Failed UMETA(DisplayName = "Failed"),
	
	/** Task is in an invalid state and cannot be executed */
	Invalid UMETA(DisplayName = "Invalid")
};

/**
 * Forward declarations for types we'll reference
 */
class UHTNTask;
class UHTNPrimitiveTask;
class UHTNCompoundTask;
class UHTNMethod;