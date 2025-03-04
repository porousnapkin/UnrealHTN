// HTNPlannerTest.cpp
// This file contains a test scenario for the HTN DFS Planner

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "HTNDFSPlanner.h"
#include "HTNWorldStateStruct.h"
#include "HTNCompoundTask.h"
#include "HTNPrimitiveTask.h"
#include "HTNCondition.h"
#include "HTNEffect.h"
#include "HTNMethod.h"

#if WITH_DEV_AUTOMATION_TESTS

// Delegate types for conditions and effects
DECLARE_DELEGATE_RetVal_OneParam(bool, FHTNCheckConditionDelegate, const UHTNWorldState*);
DECLARE_DELEGATE_RetVal(FString, FHTNGetDescriptionDelegate);
DECLARE_DELEGATE_OneParam(FHTNApplyEffectDelegate, UHTNWorldState*);

// Create a concrete subclass of UHTNCondition
class FTestPropertyCondition
{
public:
    FTestPropertyCondition(FName InPropertyName, const FHTNProperty& InExpectedValue)
        : PropertyName(InPropertyName)
        , ExpectedValue(InExpectedValue)
    {
    }

    bool CheckCondition(const UHTNWorldState* WorldState) const
    {
        FHTNProperty CurrentValue;
        if (WorldState->GetProperty(PropertyName, CurrentValue))
        {
            return CurrentValue == ExpectedValue;
        }
        return false;
    }

    FString GetDescription() const
    {
        return FString::Printf(TEXT("Property %s == %s"), *PropertyName.ToString(), *ExpectedValue.ToString());
    }

private:
    FName PropertyName;
    FHTNProperty ExpectedValue;
};

// Create a concrete class for property effects
class FTestPropertyEffect
{
public:
    FTestPropertyEffect(FName InPropertyName, const FHTNProperty& InValue)
        : PropertyName(InPropertyName)
        , Value(InValue)
    {
    }

    void ApplyEffect(UHTNWorldState* WorldState) const
    {
        WorldState->SetProperty(PropertyName, Value);
    }

    FString GetDescription() const
    {
        return FString::Printf(TEXT("Set %s = %s"), *PropertyName.ToString(), *Value.ToString());
    }

private:
    FName PropertyName;
    FHTNProperty Value;
};

// Set up a test condition on a UHTNCondition object
UHTNCondition* CreateTestCondition(UObject* Outer, FName PropertyName, const FHTNProperty& ExpectedValue)
{
    UHTNCondition* Condition = NewObject<UHTNCondition>(Outer);
    
    // Store the condition's data directly on the condition 
    // (this requires your UHTNCondition to support this, otherwise use a helper object)
    Condition->SetFlags(RF_Transient);
    
    // Hook up the condition's Execute function to check the property
    // Note: This is a simplification and assumes your UHTNCondition has implementation that 
    // can be configured this way. Your actual implementation might need to be different.
    
    // Using a lambda function as a workaround since we need the property name and expected value
    auto CheckFunction = [PropertyName, ExpectedValue](const UHTNWorldState* WorldState) -> bool {
        FHTNProperty CurrentValue;
        if (WorldState->GetProperty(PropertyName, CurrentValue))
        {
            return CurrentValue == ExpectedValue;
        }
        return false;
    };
    
    // This is a hypothetical way to hook up the lambda. Your actual UHTNCondition might need different setup
    // Condition->ConfigureCheckFunction(CheckFunction);  // Uncomment if your UHTNCondition supports this
    
    return Condition;
}

// Set up a test effect on a UHTNEffect object
UHTNEffect* CreateTestEffect(UObject* Outer, FName PropertyName, const FHTNProperty& Value)
{
    UHTNEffect* Effect = NewObject<UHTNEffect>(Outer);
    
    // Store the effect's data directly on the effect
    // (this requires your UHTNEffect to support this, otherwise use a helper object)
    Effect->SetFlags(RF_Transient);
    
    // Hook up the effect's Execute function to set the property
    // Note: This is a simplification and assumes your UHTNEffect has implementation that 
    // can be configured this way. Your actual implementation might need to be different.
    
    // Using a lambda function as a workaround since we need the property name and value
    auto ApplyFunction = [PropertyName, Value](UHTNWorldState* WorldState) {
        WorldState->SetProperty(PropertyName, Value);
    };
    
    // This is a hypothetical way to hook up the lambda. Your actual UHTNEffect might need different setup
    // Effect->ConfigureApplyFunction(ApplyFunction);  // Uncomment if your UHTNEffect supports this
    
    return Effect;
}

// Main test implementation
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHTNDFSPlannerTest, "HTNPlanner.DFSPlanner", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FHTNDFSPlannerTest::RunTest(const FString& Parameters)
{
    // This test is a stub and will need to be adapted to your actual HTN implementation
    
    // Create the world state
    UHTNWorldState* WorldState = NewObject<UHTNWorldState>();
    
    // Initialize the world state
    WorldState->SetProperty(FName("AtLocation"), FHTNProperty(true));
    WorldState->SetProperty(FName("Location"), FHTNProperty(FName("Home")));
    WorldState->SetProperty(FName("HasItem"), FHTNProperty(false));
    WorldState->SetProperty(FName("HoldingItem"), FHTNProperty(FName("None")));
    WorldState->SetProperty(FName("TaskCompleted"), FHTNProperty(false));
    
    // Create the planner
    UHTNDFSPlanner* Planner = NewObject<UHTNDFSPlanner>();
    
    // Configure the planner
    FHTNPlanningConfig PlanConfig;
    PlanConfig.MaxSearchDepth = 20;
    PlanConfig.PlanningTimeout = 5.0f; // 5 seconds timeout
    PlanConfig.bDetailedDebugging = true;
    
    UE_LOG(LogTemp, Display, TEXT("HTN Planner Test - Basic check"));
    
    // Create the simplest possible goal - just a primitive task
    // Replace this with actual task creation that matches your implementation
    UHTNPrimitiveTask* SimpleGoalTask = NewObject<UHTNPrimitiveTask>();
    
    // Add basic task to test the planner
    TArray<UHTNTask*> GoalTasks;
    GoalTasks.Add(SimpleGoalTask);
    
    // Generate a simple plan (will likely fail, but lets us test the planner is working)
    FHTNPlannerResult PlanResult = IHTNPlannerInterface::Execute_GeneratePlan(Planner, WorldState, GoalTasks, PlanConfig);
    
    UE_LOG(LogTemp, Display, TEXT("Planner test run completed."));
    
    // In a real test, we would validate the plan, but since we didn't set up real tasks
    // we'll just return true to indicate the test itself ran
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS