// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNDebugVisualizationComponent.h"
#include "HTNWorldStateStruct.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

UHTNDebugVisualizationComponent::UHTNDebugVisualizationComponent()
    : bEnableVisualization(true)
    , ScreenPosition(FVector2D(0.05f, 0.05f))
    , MaxTasksToDisplay(10)
    , bShowAllWorldStateProperties(true)
    , TextColor(FLinearColor::White)
    , BackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
    , TextScale(1.0f)
    , HTNComponent(nullptr)
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
}

void UHTNDebugVisualizationComponent::BeginPlay()
{
    Super::BeginPlay();

    // If no HTN component is set, try to find one on the owner
    if (!HTNComponent && GetOwner())
    {
        SetHTNComponent(GetOwner()->FindComponentByClass<UHTNComponent>());
    }
}

void UHTNDebugVisualizationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void UHTNDebugVisualizationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bEnableVisualization && HTNComponent)
    {
        DrawVisualization();
    }
}

void UHTNDebugVisualizationComponent::SetHTNComponent(UHTNComponent* InHTNComponent)
{
    HTNComponent = InHTNComponent;
}

void UHTNDebugVisualizationComponent::DrawVisualization()
{
    if (!GEngine || !HTNComponent)
        return;

    // Create visualization string
    FString VisualizationText;
    
    // Title with HTN component owner name
    FString OwnerName = HTNComponent->GetOwner() ? HTNComponent->GetOwner()->GetName() : TEXT("Unknown");
    VisualizationText += FString::Printf(TEXT("HTN Plan for %s\n"), *OwnerName);
    VisualizationText += TEXT("----------------------------------------------\n");
    
    // Add plan visualization
    VisualizationText += GetPlanVisualizationString();
    VisualizationText += TEXT("\n");
    
    // Add world state visualization
    VisualizationText += TEXT("World State:\n");
    VisualizationText += TEXT("----------------------------------------------\n");
    VisualizationText += GetWorldStateString();
    
    // Calculate position and dimensions
    UWorld* World = GetWorld();
    if (World && World->IsGameWorld())
    {
        if (ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController())
        {
            if (UGameViewportClient* ViewportClient = World->GetGameViewport())
            {
                FVector2D VectorSize;
                ViewportClient->GetViewportSize(VectorSize); 
                
                float PosX = VectorSize.X * ScreenPosition.X;
                float PosY = VectorSize.Y * ScreenPosition.Y;
                
                // Draw the background box (slightly larger than text)
                // GEngine->AddOnScreenDebugMessage(
                //     BaseTextKey - 1, // Unique key for this message
                //     0.0f, // Persistent until next update
                //     BackgroundColor.ToFColor(true),
                //     VisualizationText,
                //     true, // New line
                //     //FVector2D(PosX, PosY),
                //     TextScale);
                
                // Draw the text
                GEngine->AddOnScreenDebugMessage(
                    BaseTextKey, // Unique key for this message
                    0.0f, // Persistent until next update
                    TextColor.ToFColor(true),
                    VisualizationText,
                    true, // New line
                    //FVector2D(PosX, PosY),
                    FVector2d(TextScale, TextScale));
            }
        }
    }
}

FString UHTNDebugVisualizationComponent::GetPlanVisualizationString() const
{
    FString Result;
    
    if (!HTNComponent)
        return TEXT("No HTN Component");
    
    const FHTNPlan& CurrentPlan = HTNComponent->GetCurrentPlan();
    
    if (CurrentPlan.IsEmpty())
    {
        Result += TEXT("No Active Plan\n");
        return Result;
    }
    
    // Plan status
    FString StatusStr;
    switch (CurrentPlan.Status)
    {
        case EHTNPlanStatus::NotStarted:
            StatusStr = TEXT("Not Started");
            break;
        case EHTNPlanStatus::Executing:
            StatusStr = FString::Printf(TEXT("Executing (Task %d/%d)"), CurrentPlan.CurrentTaskIndex + 1, CurrentPlan.Tasks.Num());
            break;
        case EHTNPlanStatus::Paused:
            StatusStr = FString::Printf(TEXT("Paused (Task %d/%d)"), CurrentPlan.CurrentTaskIndex + 1, CurrentPlan.Tasks.Num());
            break;
        case EHTNPlanStatus::Completed:
            StatusStr = TEXT("Completed");
            break;
        case EHTNPlanStatus::Failed:
            StatusStr = TEXT("Failed");
            break;
        case EHTNPlanStatus::Aborted:
            StatusStr = TEXT("Aborted");
            break;
        default:
            StatusStr = TEXT("Unknown");
            break;
    }
    
    Result += FString::Printf(TEXT("Plan Status: %s\n"), *StatusStr);
    Result += FString::Printf(TEXT("Total Tasks: %d, Current: %d\n"), CurrentPlan.Tasks.Num(), CurrentPlan.CurrentTaskIndex);
    
    // Task list
    Result += TEXT("\nTasks:\n");
    
    int32 StartIndex = FMath::Max(0, CurrentPlan.CurrentTaskIndex - 2);
    int32 EndIndex = FMath::Min(CurrentPlan.Tasks.Num() - 1, StartIndex + MaxTasksToDisplay - 1);
    
    for (int32 i = StartIndex; i <= EndIndex; ++i)
    {
        const UHTNPrimitiveTask* Task = CurrentPlan.Tasks[i];
        if (!Task)
            continue;
        
        FString TaskStatusSymbol;
        if (i < CurrentPlan.CurrentTaskIndex)
        {
            TaskStatusSymbol = TEXT("[✓] "); // Completed
        }
        else if (i == CurrentPlan.CurrentTaskIndex && CurrentPlan.bIsExecuting)
        {
            if (CurrentPlan.bIsPaused)
            {
                TaskStatusSymbol = TEXT("[⏸] "); // Paused
            }
            else
            {
                TaskStatusSymbol = TEXT("[►] "); // In Progress
            }
        }
        else
        {
            TaskStatusSymbol = TEXT("[ ] "); // Pending
        }
        
        Result += FString::Printf(TEXT("%s%d: %s\n"), 
            *TaskStatusSymbol, i + 1, *Task->TaskName.ToString());
    }
    
    // Show ellipsis if there are more tasks beyond what we're displaying
    if (EndIndex < CurrentPlan.Tasks.Num() - 1)
    {
        Result += TEXT("...\n");
    }
    
    return Result;
}

FString UHTNDebugVisualizationComponent::GetWorldStateString() const
{
    FString Result;
    
    if (!HTNComponent || !HTNComponent->GetWorldState())
        return TEXT("No World State");
    
    UHTNWorldState* WorldState = HTNComponent->GetWorldState();
    TArray<FName> PropertyNames = WorldState->GetPropertyNames();
    
    // Filter properties if needed
    if (!bShowAllWorldStateProperties && PropertiesToDisplay.Num() > 0)
    {
        PropertyNames = PropertyNames.FilterByPredicate([this](const FName& PropertyName) {
            return PropertiesToDisplay.Contains(PropertyName);
        });
    }
    
    // Sort alphabetically for ease of reading
    PropertyNames.Sort([](const FName& A, const FName& B) {
        return A.ToString() < B.ToString();
    });
    
    // List all properties
    if (PropertyNames.Num() == 0)
    {
        Result += TEXT("(No properties)\n");
    }
    else
    {
        for (const FName& PropertyName : PropertyNames)
        {
            FHTNProperty PropertyValue;
            if (WorldState->GetProperty(PropertyName, PropertyValue))
            {
                Result += FString::Printf(TEXT("%s: %s\n"), *PropertyName.ToString(), *PropertyValue.ToString());
            }
        }
    }
    
    return Result;
}

FColor UHTNDebugVisualizationComponent::GetTaskStatusColor(EHTNTaskStatus Status) const
{
    switch (Status)
    {
        case EHTNTaskStatus::Succeeded:
            return FColor::Green;
        case EHTNTaskStatus::Failed:
            return FColor::Red;
        case EHTNTaskStatus::InProgress:
            return FColor::Yellow;
        case EHTNTaskStatus::Invalid:
        default:
            return FColor::White;
    }
}