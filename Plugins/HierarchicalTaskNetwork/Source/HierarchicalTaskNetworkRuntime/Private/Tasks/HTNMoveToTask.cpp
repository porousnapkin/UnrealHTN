#include "Tasks/HTNMoveToTask.h"
#include "HTNWorldStateStruct.h"
#include "HTNExecutionContext.h"
#include "HTNLogging.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

UHTNMoveToTask::UHTNMoveToTask()
    : bUseParameterLocation(false)
    , DestinationParameterName(NAME_None)
    , DestinationPropertyKey(NAME_None)
    , FixedDestination(FVector::ZeroVector)
    , AcceptanceRadius(50.0f)
    , bProjectDestinationToNavigation(true)
    , bAllowPartialPath(true)
    , bUsePathfinding(true)
    , MovementSpeed(0.0f)
    , MoveRequestID(FAIRequestID::InvalidRequest)
    , PathComputationWaitTime(0.0f)
{
    TaskName = FName("MoveTo");
    DebugColor = FLinearColor(0.0f, 0.7f, 1.0f); // Cyan blue for movement
}

EHTNTaskStatus UHTNMoveToTask::ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext)
{
    // Try to get the AI controller from the world state's owner actor
    AAIController* Controller = GetController(ExecutionContext->GetWorldState());
    if (!Controller)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: No AI controller found"));
        return EHTNTaskStatus::Failed;
    }

    // Get the destination
    FVector Destination;
    if (!GetDestination(ExecutionContext, Destination))
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: Cannot determine destination"));
        return EHTNTaskStatus::Failed;
    }

    // Setup move request
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(Destination);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    MoveRequest.SetProjectGoalLocation(bProjectDestinationToNavigation);
    MoveRequest.SetAllowPartialPath(bAllowPartialPath);
    MoveRequest.SetUsePathfinding(bUsePathfinding);

    if (MovementSpeed > 0.0f)
    {
        // Set custom movement speed on the pawn's movement component
        APawn* ControlledPawn = Controller->GetPawn();
        if (ControlledPawn)
        {
            UPawnMovementComponent* MovementComp = ControlledPawn->GetMovementComponent();
            if (MovementComp)
            {
                // Handle different movement component types
                if (UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(MovementComp))
                {
                    CharMoveComp->MaxWalkSpeed = MovementSpeed;
                }
                else if (UFloatingPawnMovement* FloatMoveComp = Cast<UFloatingPawnMovement>(MovementComp))
                {
                    FloatMoveComp->MaxSpeed = MovementSpeed;
                }
                else
                {
                    // For custom movement components, let the controller use its default speed
                    UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task: Cannot set speed on unsupported movement component type"));
                }
            }
        }
        
        // Configure how the acceptance radius is calculated
        MoveRequest.SetReachTestIncludesAgentRadius(false);
        MoveRequest.SetReachTestIncludesGoalRadius(false);
    }


    //Listen for move finished
    Controller->ReceiveMoveCompleted.AddDynamic(this, &UHTNMoveToTask::OnMoveFinished);
    DidFinish = false;
    
    // Start the movement
    FPathFollowingRequestResult RequestResult = Controller->MoveTo(MoveRequest);
    
    if (RequestResult.Code == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: Path request failed"));
        return EHTNTaskStatus::Failed;
    }
    else if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("MoveTo task: Already at goal"));
        return EHTNTaskStatus::Succeeded;
    }

    // Store the move request ID for later checking
    MoveRequestID = RequestResult.MoveId;
    PathComputationWaitTime = 0.0f;

    UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("MoveTo task started: Moving to %s"), *Destination.ToString());
    return EHTNTaskStatus::InProgress;
}

EHTNTaskStatus UHTNMoveToTask::TickTask_Implementation(UHTNExecutionContext* ExecutionContext, float DeltaTime)
{
    if(DidFinish)
    {
        if(ResultOfPathing == EPathFollowingResult::Type::Success)
        {
            return EHTNTaskStatus::Succeeded;
        }
        else
        {
            if(ResultOfPathing == EPathFollowingResult::Type::Blocked)
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: Blocked"));
            if(ResultOfPathing == EPathFollowingResult::Type::OffPath)
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: OffPath"));
            if(ResultOfPathing == EPathFollowingResult::Type::Aborted)
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: Aborted"));
            if(ResultOfPathing == EPathFollowingResult::Type::Invalid)
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: Invalid"));
            return EHTNTaskStatus::Failed;
        }
    }
    
    // Get the controller from the world state's owner
    AAIController* Controller = GetController(ExecutionContext->GetWorldState());
    if (!Controller)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: No AI controller found"));
        return EHTNTaskStatus::Failed;
    }

    UPathFollowingComponent* PathFollowingComp = Controller->GetPathFollowingComponent();
    if (!PathFollowingComp)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: No path following component found"));
        return EHTNTaskStatus::Failed;
    }

    // Check if the movement is still valid
    if (MoveRequestID == FAIRequestID::InvalidRequest)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed during tick: Invalid move request ID"));
        return EHTNTaskStatus::Failed;
    }

    // If the move is still being processed (path is being created), wait for a reasonable time
    if (PathFollowingComp->GetStatus() == EPathFollowingStatus::Waiting)
    {
        PathComputationWaitTime += DeltaTime;
        
        // If we've waited too long for path computation, fail the task
        if (PathComputationWaitTime > 3.0f)  // 3 seconds is a reasonable timeout
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: Path computation timed out"));
            return EHTNTaskStatus::Failed;
        }
        
        return EHTNTaskStatus::InProgress;
    }

    // Check for completion or failure
    EPathFollowingStatus::Type PathFollowingStatus = PathFollowingComp->GetStatus();
    
    if (PathFollowingStatus == EPathFollowingStatus::Idle)
    {
        // Movement finished, but we need to determine if it was successful
    
        // Get the current location of the pawn
        APawn* ControlledPawn = Controller->GetPawn();
        if (!ControlledPawn)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: Controlled pawn is no longer valid"));
            return EHTNTaskStatus::Failed;
        }
    
        // Get the target location
        FVector Destination;
        if (!GetDestination(ExecutionContext, Destination))
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: Cannot determine destination for validation"));
            return EHTNTaskStatus::Failed;
        }
    
        // Check distance to destination
        float DistanceToGoal = FVector::Distance(ControlledPawn->GetActorLocation(), Destination);
        if (DistanceToGoal <= AcceptanceRadius)
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("MoveTo task succeeded: Within acceptance radius of destination"));
            return EHTNTaskStatus::Succeeded;
        }
        else
        {
            // We're idle but not at the destination, which means something failed
            FString ErrorMsg = TEXT("Failed to reach destination");
        
            // Check if we had a valid path at all
            if (!PathFollowingComp->HasValidPath())
            {
                ErrorMsg = TEXT("Invalid path");
            }
        
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task failed: %s"), *ErrorMsg);
            return EHTNTaskStatus::Failed;
        }
    }
    
    // If the path is invalid or blocked, fail the task
    if (PathFollowingStatus == EPathFollowingStatus::Paused)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task: Path following is paused"));
    }
    else if (PathFollowingStatus == EPathFollowingStatus::Waiting)
    {
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("MoveTo task: Waiting for path to become available"));
    }
    
    // Still in progress
    return EHTNTaskStatus::InProgress;
}

void UHTNMoveToTask::EndTask_Implementation(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus)
{
    Super::EndTask_Implementation(ExecutionContext, FinalStatus);
    
    // Clean up the move request if needed
    AAIController* Controller = GetController(ExecutionContext->GetWorldState());
    if (Controller && MoveRequestID != FAIRequestID::InvalidRequest)
    {
        Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UHTNMoveToTask::OnMoveFinished);
        
        // Only abort if we failed or the request is still active
        if (FinalStatus != EHTNTaskStatus::Succeeded)
        {
            Controller->StopMovement();
        }
        
        // Reset the request ID
        MoveRequestID = FAIRequestID::InvalidRequest;
    }
}

bool UHTNMoveToTask::IsApplicable(const UHTNWorldState* WorldState) const
{
    // First check the base conditions (preconditions)
    if (!Super::IsApplicable(WorldState))
    {
        return false;
    }
    
    // Check if we can get a controller - uses the world state's owner now
    AAIController* Controller = GetController(WorldState);
    if (!Controller)
    {
        return false;
    }
    
    // Check if we have a valid pawn
    APawn* ControlledPawn = Controller->GetPawn();
    if (!ControlledPawn)
    {
        return false;
    }
    
    // Check if the pawn can actually move
    UPawnMovementComponent* MovementComp = ControlledPawn->GetMovementComponent();
    if (!MovementComp || !MovementComp->IsActive())
    {
        return false;
    }
    
    // Check if we can determine a destination
    FVector Destination;
    if (!GetDestination(WorldState, Destination))
    {
        return false;
    }
    
    // Check if the destination is navigable
    if (bUsePathfinding && bProjectDestinationToNavigation)
    {
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld());
        if (!NavSys)
        {
            return false;
        }
        
        // Check if the destination can be projected to the nav mesh
        FNavLocation ProjectedLocation;
        if (!NavSys->ProjectPointToNavigation(Destination, ProjectedLocation))
        {
            return false;
        }
    }
    
    return true;
}

bool UHTNMoveToTask::ValidateTask_Implementation() const
{
    // Check base validation
    if (!Super::ValidateTask_Implementation())
    {
        return false;
    }
    
    // Check location specification logic
    if (!bUseParameterLocation && DestinationPropertyKey.IsNone() && FixedDestination.IsZero())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task validation failed: No destination specified"));
        return false;
    }
    
    if (bUseParameterLocation && DestinationParameterName.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("MoveTo task validation failed: Using parameter but no parameter name specified"));
        return false;
    }
    
    return true;
}

AAIController* UHTNMoveToTask::GetController(const UHTNWorldState* WorldState) const
{
    // Get owner actor from world state
    AActor* Owner = WorldState ? WorldState->GetOwner() : nullptr;
    if (!Owner)
    {
        return nullptr;
    }
    
    // Check if the owner is already an AI controller
    if (AAIController* Controller = Cast<AAIController>(Owner))
    {
        return Controller;
    }
    
    // Check if owner is a pawn with an AI controller
    if (APawn* Pawn = Cast<APawn>(Owner))
    {
        return Cast<AAIController>(Pawn->GetController());
    }
    
    return nullptr;
}

bool UHTNMoveToTask::GetDestination(const UHTNExecutionContext* ExecutionContext, FVector& OutDestination) const
{
    // Get location from parameter
    if (bUseParameterLocation)
    {
        FHTNProperty LocationProperty;
        if (ExecutionContext->GetParameter(DestinationParameterName, LocationProperty))
        {
            if (LocationProperty.GetType() == EHTNPropertyType::Vector)
            {
                OutDestination = LocationProperty.GetVectorValue();
                return true;
            }
        }
        return false;
    }
    
    // Get location from world state
    if (!DestinationPropertyKey.IsNone())
    {
        FHTNProperty LocationProperty;
        if (ExecutionContext->GetWorldState()->GetProperty(DestinationPropertyKey, LocationProperty))
        {
            if (LocationProperty.GetType() == EHTNPropertyType::Vector)
            {
                OutDestination = LocationProperty.GetVectorValue();
                return true;
            }
        }
        return false;
    }
    
    // Use fixed location
    if (!FixedDestination.IsZero())
    {
        OutDestination = FixedDestination;
        return true;
    }
    
    return false;
}

// Overload for world state (used during planning/validation)
bool UHTNMoveToTask::GetDestination(const UHTNWorldState* WorldState, FVector& OutDestination) const
{
    // Get location from world state
    if (!DestinationPropertyKey.IsNone())
    {
        FHTNProperty LocationProperty;
        if (WorldState->GetProperty(DestinationPropertyKey, LocationProperty))
        {
            if (LocationProperty.GetType() == EHTNPropertyType::Vector)
            {
                OutDestination = LocationProperty.GetVectorValue();
                return true;
            }
        }
        return false;
    }
    
    // Use fixed location
    if (!FixedDestination.IsZero())
    {
        OutDestination = FixedDestination;
        return true;
    }
    
    return false;
}

void UHTNMoveToTask::OnMoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if(MoveRequestID)
    {
        DidFinish = true;
        ResultOfPathing = Result;
    }
}
