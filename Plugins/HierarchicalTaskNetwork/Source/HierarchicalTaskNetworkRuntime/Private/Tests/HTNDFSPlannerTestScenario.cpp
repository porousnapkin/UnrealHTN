// Copyright Epic Games, Inc. All Rights Reserved.

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

// Simple test implementation that uses fewer UObjects and less dynamic creation
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHTNDFSPlannerTest, "HTNPlanner.DFSPlanner.SimpleScenario", 
                               EAutomationTestFlags::ApplicationContextMask | 
                               EAutomationTestFlags::EngineFilter)

bool FHTNDFSPlannerTest::RunTest(const FString& Parameters)
{
    UE_LOG(LogTemp, Display, TEXT("HTN DFS Planner Test - Beginning simple scenario"));
    
    // Create a test world state
    UHTNWorldState* WorldState = NewObject<UHTNWorldState>();
    
    // Initialize with some test data
    WorldState->SetPropertyValue<FName>("Location", FName("Start"));
    WorldState->SetPropertyValue<bool>("HasKey", false);
    WorldState->SetPropertyValue<bool>("DoorOpen", false);
    
    UE_LOG(LogTemp, Display, TEXT("Initial world state:"));
    UE_LOG(LogTemp, Display, TEXT("%s"), *WorldState->ToString());
    
    // Create a planner
    UHTNDFSPlanner* Planner = NewObject<UHTNDFSPlanner>();
    
    // Configure with simple settings
    FHTNPlanningConfig PlanConfig;
    PlanConfig.MaxSearchDepth = 10;
    PlanConfig.PlanningTimeout = 1.0f;
    PlanConfig.bDetailedDebugging = true;
    
    // Instead of creating complex hierarchies, we'll just check the planner's basic functionality
    
    // Create a simple goal task that will trivially succeed
    UHTNPrimitiveTask* SimpleTask = NewObject<UHTNPrimitiveTask>();
    SimpleTask->TaskName = FName("TestTask");
    
    // Create a goal array with just this task
    TArray<UHTNTask*> GoalTasks;
    GoalTasks.Add(SimpleTask);
    
    // Run the planner - this is just a basic sanity check
    FHTNPlannerResult PlanResult = Planner->GeneratePlan(WorldState, GoalTasks, PlanConfig);
    
    // We're just making sure the planner doesn't crash and returns something
    UE_LOG(LogTemp, Display, TEXT("Planner ran without crashes"));
    
    // Test that the planner interface methods don't crash
    if (PlanResult.bSuccess)
    {
        TestTrue("Did planner validate the results", Planner->ValidatePlan(PlanResult.Plan, WorldState));
        UE_LOG(LogTemp, Display, TEXT("ValidatePlan ran without crashes"));
    }
    
    UE_LOG(LogTemp, Display, TEXT("HTN DFS Planner test completed. This test only verified the planner doesn't crash."));
    
    // Return true because we're just checking it doesn't crash
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS