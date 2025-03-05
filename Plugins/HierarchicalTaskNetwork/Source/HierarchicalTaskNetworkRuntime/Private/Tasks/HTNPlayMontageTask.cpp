#include "Tasks/HTNPlayMontageTask.h"
#include "HTNExecutionContext.h"
#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UHTNPlayMontageTask::UHTNPlayMontageTask()
    : Montage(nullptr)
    , bUseMontageFromWorldState(false)
    , MontagePropertyKey(NAME_None)
    , PlayRate(1.0f)
    , bWaitForCompletion(true)
    , bFailWhenNotPlayed(true)
    , StartSection(NAME_None)
    , StartPosition(0.0f)
    , BlendInTime(0.25f)
    , BlendOutTime(0.25f)
    , bNotifyAnimationComplete(false)
    , bStoreMontageLength(false)
    , MontageLengthKey(NAME_None)
    , ActiveMontage(nullptr)
    , bMontageStarted(false)
    , bMontageCompleted(false)
{
    TaskName = FName("PlayMontage");
    DebugColor = FLinearColor(0.8f, 0.2f, 0.8f); // Purple for animation tasks
}

EHTNTaskStatus UHTNPlayMontageTask::ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext)
{
    if (!ExecutionContext || !ExecutionContext->GetWorldState())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No execution context or world state"));
        return EHTNTaskStatus::Failed;
    }

    // Get the actor
    AActor* TargetActor = ExecutionContext->GetOwner();
    if (!TargetActor)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No target actor"));
        return EHTNTaskStatus::Failed;
    }

    // Get the animation montage to play
    UAnimMontage* MontageToPlay = nullptr;
    
    if (bUseMontageFromWorldState)
    {
        // Get montage from world state
        FHTNProperty MontageProperty;
        if (ExecutionContext->GetWorldState()->GetProperty(MontagePropertyKey, MontageProperty) && 
            MontageProperty.GetType() == EHTNPropertyType::Object)
        {
            MontageToPlay = Cast<UAnimMontage>(MontageProperty.GetObjectValue());
        }
        
        if (!MontageToPlay)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: Failed to get montage from world state"));
            return bFailWhenNotPlayed ? EHTNTaskStatus::Failed : EHTNTaskStatus::Succeeded;
        }
    }
    else
    {
        // Use the set montage
        MontageToPlay = Montage;
        
        if (!MontageToPlay)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No montage set"));
            return bFailWhenNotPlayed ? EHTNTaskStatus::Failed : EHTNTaskStatus::Succeeded;
        }
    }

    // Get the anim instance
    UAnimInstance* AnimInstance = GetAnimInstance(TargetActor);
    if (!AnimInstance)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: Failed to find anim instance for actor %s"), *TargetActor->GetName());
        return bFailWhenNotPlayed ? EHTNTaskStatus::Failed : EHTNTaskStatus::Succeeded;
    }

    // Register delegate for montage end
    RegisterMontageEndDelegate(AnimInstance);

    // Play the montage
    float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartPosition, false);
    
    // If montage length is 0, montage failed to play
    if (MontageLength <= 0.0f)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: Failed to play montage %s"), *MontageToPlay->GetName());
        UnregisterMontageEndDelegate(AnimInstance);
        return bFailWhenNotPlayed ? EHTNTaskStatus::Failed : EHTNTaskStatus::Succeeded;
    }

    // Jump to specific section if specified
    if (StartSection != NAME_None)
    {
        AnimInstance->Montage_JumpToSection(StartSection, MontageToPlay);
    }

    // Keep track of the active montage
    ActiveMontage = MontageToPlay;
    bMontageStarted = true;
    
    // Store the montage length in the execution context if desired
    if (bStoreMontageLength && MontageLengthKey != NAME_None)
    {
        ExecutionContext->SetParameterValue<float>(MontageLengthKey, MontageLength);
    }

    UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("PlayMontageTask: Started playing montage %s (length: %.2f)"), 
        *MontageToPlay->GetName(), MontageLength);

    // If we're not waiting for completion, succeed immediately
    if (!bWaitForCompletion)
    {
        bMontageCompleted = true;
        return EHTNTaskStatus::Succeeded;
    }

    // Otherwise, return in progress
    return EHTNTaskStatus::InProgress;
}

EHTNTaskStatus UHTNPlayMontageTask::TickTask_Implementation(UHTNExecutionContext* ExecutionContext, float DeltaTime)
{
    // Check if the montage has completed - this could be set by the OnMontageEnded delegate
    if (bMontageCompleted)
    {
        return EHTNTaskStatus::Succeeded;
    }

    // If the montage hasn't started or we're not waiting for completion, nothing to tick
    if (!bMontageStarted || !bWaitForCompletion)
    {
        return EHTNTaskStatus::InProgress;
    }

    // Get the actor
    AActor* TargetActor = ExecutionContext ? ExecutionContext->GetOwner() : nullptr;
    if (!TargetActor)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No target actor during tick"));
        return EHTNTaskStatus::Failed;
    }

    // Get the anim instance
    UAnimInstance* AnimInstance = GetAnimInstance(TargetActor);
    if (!AnimInstance)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: Failed to find anim instance during tick"));
        return EHTNTaskStatus::Failed;
    }

    // Check if the montage is still playing - this is a fallback in case the delegate doesn't fire
    if (ActiveMontage && !AnimInstance->Montage_IsPlaying(ActiveMontage))
    {
        // Montage is no longer playing
        bMontageCompleted = true;
        
        // Apply effects if notifying on animation complete
        if (bNotifyAnimationComplete)
        {
            ApplyEffects(ExecutionContext);
        }
        
        return EHTNTaskStatus::Succeeded;
    }

    // Keep the task running
    return EHTNTaskStatus::InProgress;
}

void UHTNPlayMontageTask::EndTask_Implementation(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus)
{
    // Make sure to apply effects if the task succeeded
    if (FinalStatus == EHTNTaskStatus::Succeeded)
    {
        Super::EndTask_Implementation(ExecutionContext, FinalStatus);
    }

    // If the montage is still playing and the task is ending, stop it
    if (bMontageStarted && !bMontageCompleted)
    {
        // Get the actor
        AActor* TargetActor = ExecutionContext ? ExecutionContext->GetOwner() : nullptr;
        if (TargetActor)
        {
            // Get the anim instance
            UAnimInstance* AnimInstance = GetAnimInstance(TargetActor);
            if (AnimInstance)
            {
                // If we know the montage, stop it
                if (!bUseMontageFromWorldState && Montage)
                {
                    AnimInstance->Montage_Stop(BlendOutTime, Montage);
                }
                
                // Clean up the delegate
                UnregisterMontageEndDelegate(AnimInstance);
            }
        }
    }

    // Reset state
    bMontageStarted = false;
    bMontageCompleted = false;
    ActiveMontage = nullptr;
}

bool UHTNPlayMontageTask::IsApplicable(const UHTNWorldState* WorldState) const
{
    // First check the base conditions (preconditions)
    if (!Super::IsApplicable(WorldState))
    {
        return false;
    }
    
    // Check if we have a valid actor
    AActor* TargetActor = WorldState ? WorldState->GetOwner() : nullptr;
    if (!TargetActor)
    {
        return false;
    }
    
    // Check for a valid montage
    if (!bUseMontageFromWorldState)
    {
        if (!Montage)
        {
            return false;
        }
    }
    else
    {
        // Check if the montage property exists in the world state
        FHTNProperty MontageProperty;
        if (!WorldState->GetProperty(MontagePropertyKey, MontageProperty) || 
            MontageProperty.GetType() != EHTNPropertyType::Object ||
            !Cast<UAnimMontage>(MontageProperty.GetObjectValue()))
        {
            return false;
        }
    }
    
    // Check for a valid anim instance
    return GetAnimInstance(TargetActor) != nullptr;
}

bool UHTNPlayMontageTask::ValidateTask_Implementation() const
{
    // Check base validation
    if (!Super::ValidateTask_Implementation())
    {
        return false;
    }
    
    // Validate based on selected options
    if (!bUseMontageFromWorldState)
    {
        if (!Montage)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No montage set when not using world state"));
            return false;
        }
    }
    else
    {
        if (MontagePropertyKey.IsNone())
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No montage property key set when using world state"));
            return false;
        }
    }
    
    // Validate other parameters
    if (PlayRate <= 0.0f)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: Play rate must be positive"));
        return false;
    }
    
    // Validate montage length key if used
    if (bStoreMontageLength && MontageLengthKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PlayMontageTask: No montage length key set when storing length"));
        return false;
    }
    
    return true;
}

UAnimInstance* UHTNPlayMontageTask::GetAnimInstance(const AActor* TargetActor) const
{
    if (!TargetActor)
    {
        return nullptr;
    }
    
    // Try to get anim instance from character
    if (const ACharacter* Character = Cast<ACharacter>(TargetActor))
    {
        if (USkeletalMeshComponent* Mesh = Character->GetMesh())
        {
            return Mesh->GetAnimInstance();
        }
    }
    
    // Try to get skeletal mesh component directly
    USkeletalMeshComponent* SkeletalMesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>();
    if (SkeletalMesh)
    {
        return SkeletalMesh->GetAnimInstance();
    }
    
    return nullptr;
}

void UHTNPlayMontageTask::RegisterMontageEndDelegate(UAnimInstance* AnimInstance)
{
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &UHTNPlayMontageTask::OnMontageEnded);
    }
}

void UHTNPlayMontageTask::UnregisterMontageEndDelegate(UAnimInstance* AnimInstance)
{
    if (AnimInstance)
    {
        AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHTNPlayMontageTask::OnMontageEnded);
    }
}

void UHTNPlayMontageTask::OnMontageEnded(UAnimMontage* InMontage, bool bInterrupted)
{
    // Check if this is the montage we care about
    if ((bUseMontageFromWorldState || InMontage == Montage || InMontage == ActiveMontage) && 
        bMontageStarted && !bMontageCompleted)
    {
        bMontageCompleted = true;
        
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("PlayMontageTask: Montage %s completed (interrupted: %s)"), 
            *InMontage->GetName(), bInterrupted ? TEXT("true") : TEXT("false"));
        
        // The task will complete on the next tick, which will handle task status transitions
    }
}