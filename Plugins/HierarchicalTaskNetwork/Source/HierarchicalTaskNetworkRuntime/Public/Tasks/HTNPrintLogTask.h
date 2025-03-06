#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "HTNPrintLogTask.generated.h"

/**
 * Enum for debug message display options.
 */
UENUM(BlueprintType)
enum class EHTNLogDisplayMode : uint8
{
    /** Display on screen only */
    Screen UMETA(DisplayName = "Screen"),
    
    /** Print to log only */
    Log UMETA(DisplayName = "Log"),
    
    /** Display on screen and print to log */
    Both UMETA(DisplayName = "Both")
};

/**
 * Enum for debug message verbosity levels.
 */
UENUM(BlueprintType)
enum class EHTNLogVerbosity : uint8
{
    /** Display as information */
    Info UMETA(DisplayName = "Info"),
    
    /** Display as warning */
    Warning UMETA(DisplayName = "Warning"),
    
    /** Display as error */
    Error UMETA(DisplayName = "Error")
};

/**
 * HTN primitive task that prints a debug message.
 * Useful for debugging HTN plan execution.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPrintLogTask : public UHTNPrimitiveTask
{
    GENERATED_BODY()

public:
    UHTNPrintLogTask();

    //~ Begin UHTNPrimitiveTask Interface
    virtual EHTNTaskStatus ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext) override;
    virtual bool ValidateTask_Implementation() const override;
    //~ End UHTNPrimitiveTask Interface

    /** The message to display */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug", meta = (MultiLine = true))
    FString Message;
    
    /** Whether to replace {PropertyName} tokens with world state values */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug")
    bool bIncludeWorldStateValues;
    
    /** How to display the message */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug")
    EHTNLogDisplayMode DisplayMode;
    
    /** Verbosity level of the message */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug")
    EHTNLogVerbosity Verbosity;
    
    /** How long to display the on-screen message (in seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug", meta = (EditCondition = "DisplayMode != EHTNLogDisplayMode::Log", ClampMin = "0.1"))
    float DisplayDuration;
    
    /** Color for the on-screen message */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Debug", meta = (EditCondition = "DisplayMode != EHTNLogDisplayMode::Log"))
    FLinearColor DisplayColor;

private:
    /** Process the message text, replacing tokens with actual world state values */
    FString ProcessMessageText(const UHTNWorldState* WorldState) const;
    
    /** Display the message with the appropriate display mode and verbosity */
    void DisplayMessage(const FString& FormattedMessage) const;
};