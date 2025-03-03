// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNPropertyType.generated.h"

/**
 * Enumeration of property types that can be stored in an HTN world state.
 * These represent the different data types that can be used for planning operations.
 */
UENUM(BlueprintType)
enum class EHTNPropertyType : uint8
{
	/** Boolean property (true/false) */
	Boolean UMETA(DisplayName = "Boolean"),
	
	/** Integer property (whole number) */
	Integer UMETA(DisplayName = "Integer"),
	
	/** Float property (decimal number) */
	Float UMETA(DisplayName = "Float"),
	
	/** String property (text) */
	String UMETA(DisplayName = "String"),
	
	/** Name property (FName, optimized for lookups) */
	Name UMETA(DisplayName = "Name"),
	
	/** Object reference property (UObject pointer) */
	Object UMETA(DisplayName = "Object Reference"),
	
	/** Vector property (FVector, for positions) */
	Vector UMETA(DisplayName = "Vector"),
	
	/** Invalid property type for error handling */
	Invalid UMETA(DisplayName = "Invalid", Hidden)
};