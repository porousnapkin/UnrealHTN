#include "HTNComponent.h"

#include "HTNDebugVisualizationComponent.h"
#include "HTNLogging.h"
#include "Tasks/HTNTask.h"
#include "HTNPlanExecutor.h"
#include "HTNDFSPlanner.h"

UHTNComponent::UHTNComponent()
    : bDebugOutput(false)
    , bAutoReplanEnabled(true)
    , ReplanCheckInterval(0.5f)
    , LastReplanCheckTime(0.0f)
    , ConsecutivePlanFailures(0)
{
    // Set this component to be initialized when the game starts, and to be ticked every frame
    PrimaryComponentTick.bCanEverTick = true;
}

void UHTNComponent::BeginPlay()
{
    Super::BeginPlay();

    Planner = NewObject<UHTNDFSPlanner>(const_cast<UHTNComponent*>(this));
    if (!Planner)
    {
        DebugMessage(TEXT("Failed to create planner"));
    }
    
    // Initialize the component
    Initialize();
}

void UHTNComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup
    if (PlanExecutor && PlanExecutor->IsExecutingPlan())
    {
        PlanExecutor->AbortPlan(false);
    }
    
    Super::EndPlay(EndPlayReason);
}

void UHTNComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // If we have a plan executor, allow it to tick
    if (PlanExecutor)
    {
        // The plan executor handles its own ticking through FTickableGameObject
        // so we don't need to call any explicit tick function here
        
        // Check if we need to replan
        if (bAutoReplanEnabled)
        {
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastReplanCheckTime >= ReplanCheckInterval)
            {
                LastReplanCheckTime = CurrentTime;
                
                // Try to replan if needed
                AutoReplan();
            }
        }
        
        // Debug output
        if (bDebugOutput && PlanExecutor->IsExecutingPlan())
        {
            // Periodically output current task info for debugging
            static float LastDebugTime = 0.0f;
            float CurrentTime = GetWorld()->GetTimeSeconds();
            
            if (CurrentTime - LastDebugTime > 1.0f) // Debug output every second
            {
                LastDebugTime = CurrentTime;
                
                UHTNPrimitiveTask* CurrentTask = PlanExecutor->GetCurrentTask();
                if (CurrentTask)
                {
                    DebugMessage(FString::Printf(TEXT("Executing task: %s"), *CurrentTask->ToString()));
                }
            }
        }
    }
}

bool UHTNComponent::GeneratePlan(const TArray<UHTNTask*>& GoalTasks)
{
    if (GoalTasks.Num() == 0)
    {
        DebugMessage(TEXT("Cannot generate plan: No goal tasks provided"));
        return false;
    }
    
    if (!WorldState)
    {
        DebugMessage(TEXT("Cannot generate plan: No world state available"));
        return false;
    }
    
    // Abort any existing plan
    if (PlanExecutor && PlanExecutor->IsExecutingPlan())
    {
        PlanExecutor->AbortPlan(false);
    }

    // Make sure the world state has the owner set
    if (!WorldState->GetOwner())
    {
        WorldState->SetOwner(GetOwner());
    }

    //Create the execution context for this plan.
    ExecutionContext = NewObject<UHTNExecutionContext>();
    ExecutionContext->SetWorldState(WorldState);
    
    // Configure the planner
    FHTNPlanningConfig PlanConfig;
    PlanConfig.MaxSearchDepth = 20;
    PlanConfig.PlanningTimeout = 0.5f;
    PlanConfig.bDetailedDebugging = bDebugOutput;
    
    // Generate the plan
    FHTNPlannerResult PlanResult = Planner->GeneratePlan(WorldState, GoalTasks, PlanConfig);
    
    if (PlanResult.bSuccess)
    {
        // Save the goal tasks for potential replanning
        CurrentGoalTasks = GoalTasks;
        
        // Reset failure counter on successful planning
        ConsecutivePlanFailures = 0;
        
        DebugMessage(FString::Printf(TEXT("Plan generated successfully with %d tasks"), PlanResult.Plan.Tasks.Num()));
        
        // Start executing the plan
        if (PlanExecutor)
        {
            return PlanExecutor->StartPlan(PlanResult.Plan, ExecutionContext, GetOwner());
        }
        else
        {
            DebugMessage(TEXT("Plan executor is not available"));
            return false;
        }
    }
    else
    {
        DebugMessage(FString::Printf(TEXT("Failed to generate plan: %s"), 
            *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(PlanResult.FailReason))));
        
        // Increment failure counter
        ConsecutivePlanFailures++;
        
        // Handle plan failure if we've failed multiple times
        if (ConsecutivePlanFailures >= 3)
        {
            HandlePlanFailure();
        }
        
        return false;
    }
}

bool UHTNComponent::IsPlanValid() const
{
    if (GetCurrentPlan().IsEmpty())
    {
        return false;
    }
    
    if (!WorldState)
    {
        return false;
    }
    
    if (!Planner)
    {
        return false;
    }
    
    return Planner->ValidatePlan(GetCurrentPlan(), WorldState);
}

bool UHTNComponent::TryReplan(const TArray<UHTNTask*>& GoalTasks)
{
    // If the current plan is valid, no need to replan
    if (IsExecutingPlan() && IsPlanValid())
    {
        return true;
    }
    
    // Otherwise, generate a new plan
    return GeneratePlan(GoalTasks);
}

UHTNExecutionContext* UHTNComponent::GetExecutionContext() const
{
    return ExecutionContext;
}

UHTNWorldState* UHTNComponent::GetWorldState() const
{
    return WorldState;
}

void UHTNComponent::SetWorldState(UHTNWorldState* InWorldState)
{
    WorldState = InWorldState;
    
    // Make sure the world state has the owner set
    if (WorldState && !WorldState->GetOwner())
    {
        WorldState->SetOwner(GetOwner());
    }
    
    // Update the execution context
    if (WorldState && ExecutionContext)
    {
        ExecutionContext->SetWorldState(WorldState);
    }
}

const FHTNPlan& UHTNComponent::GetCurrentPlan() const
{
    if (PlanExecutor)
    {
        return PlanExecutor->GetCurrentPlan();
    }

    return EmptyPlan;
}

bool UHTNComponent::IsExecutingPlan() const
{
    return PlanExecutor && PlanExecutor->IsExecutingPlan();
}

bool UHTNComponent::AbortPlan(bool bFailTasks)
{
    if (PlanExecutor && PlanExecutor->IsExecutingPlan())
    {
        return PlanExecutor->AbortPlan(bFailTasks);
    }
    
    return false;
}

FString UHTNComponent::GetDebugInfo() const
{
    FString Result = TEXT("HTN Component Debug Info:\n");
    
    // Component info
    Result += FString::Printf(TEXT("Owner: %s\n"), GetOwner() ? *GetOwner()->GetName() : TEXT("None"));
    
    // World state info
    Result += TEXT("\nWorld State:\n");
    if (WorldState)
    {
        Result += WorldState->ToString();
    }
    else
    {
        Result += TEXT("  (None)\n");
    }
    
    // Execution context info
    Result += TEXT("\nExecution Context:\n");
    if (ExecutionContext)
    {
        Result += ExecutionContext->ToString();
    }
    else
    {
        Result += TEXT("  (None)\n");
    }
    
    // Plan info
    Result += TEXT("\nCurrent Plan:\n");
    if (!GetCurrentPlan().IsEmpty())
    {
        Result += GetCurrentPlan().ToString();
    }
    else
    {
        Result += TEXT("  (No Plan)\n");
    }
    
    // Execution status
    Result += TEXT("\nExecution Status: ");
    if (IsExecutingPlan())
    {
        Result += TEXT("Executing\n");
    }
    else
    {
        Result += TEXT("Not Executing\n");
    }
    
    return Result;
}

void UHTNComponent::Initialize()
{
    // Create a world state if we don't have one
    if (!WorldState)
    {
        WorldState = NewObject<UHTNWorldState>(this);
        WorldState->SetOwner(GetOwner());
        DebugMessage(TEXT("Created new world state"));
    }
    else if (!WorldState->GetOwner())
    {
        // Ensure owner is set
        WorldState->SetOwner(GetOwner());
    }
    
    // Set up the execution context
    if (!ExecutionContext)
    {
        ExecutionContext = NewObject<UHTNExecutionContext>(this);
    }
    ExecutionContext->SetWorldState(WorldState);
    
    // Create the plan executor
    PlanExecutor = NewObject<UHTNPlanExecutor>(this);
    if (PlanExecutor)
    {
        // Default configuration
        PlanExecutor->SetExecutionMode(EHTNPlanExecutorMode::Sequential);
        PlanExecutor->SetMaxTaskExecutionTime(0.0f); // No timeout
        DebugMessage(TEXT("Created plan executor"));
    }
    else
    {
        DebugMessage(TEXT("Failed to create plan executor"));
    }
    
    // Initialize replanning variables
    LastReplanCheckTime = GetWorld()->GetTimeSeconds();
    ConsecutivePlanFailures = 0;
}

bool UHTNComponent::NeedsReplan() const
{
    // If we don't have a plan or it's empty, we need to replan
    if (GetCurrentPlan().IsEmpty())
    {
        return true;
    }
    
    // If we're not currently executing a plan, we need to replan
    if (!IsExecutingPlan())
    {
        return true;
    }
    
    // Check if the plan is still valid
    if (!IsPlanValid())
    {
        return true;
    }
    
    // No need to replan
    return false;
}

bool UHTNComponent::AutoReplan()
{
    // If we need to replan and have goal tasks, try to replan
    if (NeedsReplan() && CurrentGoalTasks.Num() > 0)
    {
        DebugMessage(TEXT("Auto-replanning triggered"));
        return TryReplan(CurrentGoalTasks);
    }
    
    // No replanning was done or needed
    return true;
}

void UHTNComponent::SetAutoReplanEnabled(bool bEnable, float CheckInterval)
{
    bAutoReplanEnabled = bEnable;
    ReplanCheckInterval = FMath::Max(0.1f, CheckInterval); // Minimum interval of 0.1 seconds
    
    DebugMessage(FString::Printf(TEXT("Auto-replanning %s (interval: %.2f seconds)"), 
        bEnable ? TEXT("enabled") : TEXT("disabled"), ReplanCheckInterval));
}

void UHTNComponent::HandlePlanFailure()
{
    // Log the failure
    DebugMessage(FString::Printf(TEXT("Handling plan failure after %d consecutive failures"), ConsecutivePlanFailures));
    
    // Implement basic recovery strategies
    
    // 1. Reset any potentially stuck state
    if (PlanExecutor && PlanExecutor->IsExecutingPlan())
    {
        PlanExecutor->AbortPlan(true);
    }
    
    // 2. Try to clear or reset problematic world state
    // This is a simple approach - in a real implementation, you might have 
    // domain-specific recovery logic here
    
    // 3. Wait a bit before trying again (implemented through no action here,
    // as the tick system will naturally wait before the next attempt)
    
    // 4. If we've had too many failures, maybe we should change goals
    if (ConsecutivePlanFailures >= 5)
    {
        DebugMessage(TEXT("Too many consecutive failures, consider changing goals"));
        // In a real implementation, you might signal to a higher-level AI system
        // that this agent needs new goals or is stuck
    }
}

void UHTNComponent::DebugMessage(const FString& Message) const
{
    if (bDebugOutput)
    {
        UE_LOG(LogHTNPlannerPlugin, Display, TEXT("[HTNComponent] %s"), *Message);
    }
}

void UHTNComponent::SetDebugVisualization(bool bEnable)
{
    bDebugOutput = bEnable;
    
    // If already has a visualization component, update its state
    if (UHTNDebugVisualizationComponent* VisComp = GetOwner()->FindComponentByClass<UHTNDebugVisualizationComponent>())
    {
        VisComp->bEnableVisualization = bEnable;
    }
}

UHTNDebugVisualizationComponent* UHTNComponent::CreateVisualizationComponent()
{
    if (!GetOwner())
    {
        DebugMessage(TEXT("Cannot create visualization component: No owner actor"));
        return nullptr;
    }
    
    // Check if a visualization component already exists
    UHTNDebugVisualizationComponent* VisComp = GetOwner()->FindComponentByClass<UHTNDebugVisualizationComponent>();
    if (!VisComp)
    {
        // Create a new visualization component
        VisComp = NewObject<UHTNDebugVisualizationComponent>(GetOwner(), UHTNDebugVisualizationComponent::StaticClass());
        VisComp->RegisterComponent();
        VisComp->SetHTNComponent(this);
        VisComp->bEnableVisualization = bDebugOutput;
        
        DebugMessage(TEXT("Created visualization component"));
    }
    else
    {
        DebugMessage(TEXT("Visualization component already exists"));
    }
    
    return VisComp;
}
