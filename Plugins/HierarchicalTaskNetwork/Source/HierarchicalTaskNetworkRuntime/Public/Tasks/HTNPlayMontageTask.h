#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Animation/AnimMontage.h"
#include "HTNPlayMontageTask.generated.h"

// Forward declarations
class USkeletalMeshComponent;
class UAnimInstance;

/**
 * HTN primitive task that plays an animation montage.
 * This task plays a specified montage on a character and waits for completion.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPlayMontageTask : public UHTNPrimitiveTask
{
    GENERATED_BODY()

public:
    UHTNPlayMontageTask();

    //~ Begin UHTNPrimitiveTask Interface
    virtual EHTNTaskStatus ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext) override;
    virtual EHTNTaskStatus TickTask_Implementation(UHTNExecutionContext* ExecutionContext, float DeltaTime) override;
    virtual void EndTask_Implementation(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus) override;
    virtual bool IsApplicable(const UHTNWorldState* WorldState) const override;
    virtual bool ValidateTask_Implementation() const override;
    //~ End UHTNPrimitiveTask Interface

protected:
    /** The animation montage to play */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    TObjectPtr<UAnimMontage> Montage;

    /** Whether to use a montage from the world state */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    bool bUseMontageFromWorldState;

    /** Property key for the montage in world state (if using world state) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (EditCondition = "bUseMontageFromWorldState"))
    FName MontagePropertyKey;

    /** The playback rate of the montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float PlayRate;

    /** Whether to wait for the montage to finish before completing the task */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    bool bWaitForCompletion;

    /** Whether the task should fail if the montage can't be played */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    bool bFailWhenNotPlayed;

    /** Section of the montage to play (if empty, plays from start) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    FName StartSection;

    /** The position to start the montage at (in seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (ClampMin = "0.0"))
    float StartPosition;

    /** Blend in time for the montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (ClampMin = "0.0"))
    float BlendInTime;

    /** Blend out time for the montage */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (ClampMin = "0.0"))
    float BlendOutTime;

    /** Whether to notify the HTN system when the animation completes via world state */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    bool bNotifyAnimationComplete;
    
    /** Whether to store the montage length in the execution context */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation")
    bool bStoreMontageLength;
    
    /** Property key to store the montage length in the execution context */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task|Animation", meta = (EditCondition = "bStoreMontageLength"))
    FName MontageLengthKey;

private:
    /** Saving this so we can use it when the delegate for the animation ending triggers*/
    UPROPERTY()
    UHTNExecutionContext* SavedExecutionContext;
    
    /** The montage currently being played */
    UAnimMontage* ActiveMontage;

    /** Whether the montage has started playing */
    bool bMontageStarted;

    /** Whether the montage has completed naturally */
    bool bMontageCompleted;

    /** Find the anim instance to use */
    UAnimInstance* GetAnimInstance(const AActor* TargetActor) const;

    /** Register a delegate to handle montage end */
    void RegisterMontageEndDelegate(UAnimInstance* AnimInstance);

    /** Unregister the montage end delegate */
    void UnregisterMontageEndDelegate(UAnimInstance* AnimInstance);

    /** Callback for when a montage ends */
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted);
};