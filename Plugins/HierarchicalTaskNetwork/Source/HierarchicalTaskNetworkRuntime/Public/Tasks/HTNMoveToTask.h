#pragma once

#include "CoreMinimal.h"
#include "HTNPrimitiveTask.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "HTNMoveToTask.generated.h"

/**
 * HTN primitive task that moves an agent to a specified location.
 * Uses the AIMoveTo functionality to navigate through the environment.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNMoveToTask : public UHTNPrimitiveTask
{
    GENERATED_BODY()

public:
    UHTNMoveToTask();

    //~ Begin UHTNPrimitiveTask Interface
    virtual EHTNTaskStatus ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext) override;
    virtual EHTNTaskStatus TickTask_Implementation(UHTNExecutionContext* ExecutionContext, float DeltaTime) override;
    virtual void EndTask_Implementation(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus) override;
    virtual bool IsApplicable(const UHTNWorldState* WorldState) const override;
    virtual bool ValidateTask_Implementation() const override;
    //~ End UHTNPrimitiveTask Interface

protected:
    /** How to specify the destination */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement")
    bool bUseParameterLocation;

    /** Parameter name for the destination (if using parameter) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement", meta = (EditCondition = "bUseParameterLocation"))
    FName DestinationParameterName;

    /** Property key for the destination in world state (if not using parameter) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement", meta = (EditCondition = "!bUseParameterLocation"))
    FName DestinationPropertyKey;

    /** Fixed location to navigate to (if not using parameter or world state) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement", meta = (EditCondition = "!bUseParameterLocation && DestinationPropertyKey.IsNone()"))
    FVector FixedDestination;

    /** How close the agent needs to get to the target (in cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement", meta = (ClampMin = "0.0"))
    float AcceptanceRadius;

    /** Whether to project the point to the navigation mesh */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement")
    bool bProjectDestinationToNavigation;

    /** Whether to allow partial paths */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement")
    bool bAllowPartialPath;

    /** Whether to use pathfinding or move directly */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement")
    bool bUsePathfinding;

    /** Maximum movement speed (0 = use default) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Movement", meta = (ClampMin = "0.0"))
    float MovementSpeed;

private:
    /** Current move request ID */
    FAIRequestID MoveRequestID;

    /** Time waiting for path computation */
    float PathComputationWaitTime;

    /** Get AI controller from world state */
    AAIController* GetController(const UHTNWorldState* WorldState) const;

    /** Get the destination from parameters or world state during execution */
    bool GetDestination(const UHTNExecutionContext* ExecutionContext, FVector& OutDestination) const;
    
    /** Get the destination from world state during planning/validation */
    bool GetDestination(const UHTNWorldState* WorldState, FVector& OutDestination) const;
};