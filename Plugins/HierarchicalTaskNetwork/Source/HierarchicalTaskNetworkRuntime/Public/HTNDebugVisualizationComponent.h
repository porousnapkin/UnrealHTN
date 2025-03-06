// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tasks/HTNTaskStatus.h"
#include "HTNComponent.h"
#include "HTNDebugVisualizationComponent.generated.h"

/**
 * Component for visualizing HTN plan execution.
 * This component displays the current plan, task status, and world state properties
 * to help debug and understand what's happening during HTN plan execution.
 */
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNDebugVisualizationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UHTNDebugVisualizationComponent();

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    /** Sets the HTN component to visualize */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN|Visualization")
    void SetHTNComponent(UHTNComponent* InHTNComponent);

    /** Whether to display the visualization */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization")
    bool bEnableVisualization;

    /** Screen position for the visualization (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    FVector2D ScreenPosition;

    /** Maximum number of tasks to display */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization", meta = (ClampMin = "1", ClampMax = "20"))
    int32 MaxTasksToDisplay;

    /** Whether to show all world state properties */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization")
    bool bShowAllWorldStateProperties;

    /** Specific world state properties to display (only used if bShowAllWorldStateProperties is false) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization", meta = (EditCondition = "!bShowAllWorldStateProperties"))
    TArray<FName> PropertiesToDisplay;

    /** Text color for the visualization */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization")
    FLinearColor TextColor = FLinearColor::Blue;

    /** Background color for the visualization */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization")
    FLinearColor BackgroundColor = FLinearColor::Black;

    /** Size of the visualization text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Visualization", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float TextScale = 1.0f;


private:
    /** The HTN component to visualize */
    UPROPERTY()
    UHTNComponent* HTNComponent;

    /** Draws the visualization on the screen */
    void DrawVisualization();

    /** Gets a string representation of the plan for visualization */
    FString GetPlanVisualizationString() const;

    /** Gets a string representation of relevant world state properties */
    FString GetWorldStateString() const;

    /** Gets a color for a particular task status */
    FColor GetTaskStatusColor(EHTNTaskStatus Status) const;
    uint64 BaseTextKey = 250;
};