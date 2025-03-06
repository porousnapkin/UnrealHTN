// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHTNComponent.h"

#include "Conditions/HTNPropertyCondition.h"
#include "Effects/HTNSetPropertyEffect.h"
#include "Tasks/HTNCompoundTask.h"
#include "Tasks/HTNMoveToTask.h"
#include "Tasks/HTNPlayMontageTask.h"
#include "Tasks/HTNPrintLogTask.h"
#include "Tasks/HTNTaskFactory.h"


// Sets default values for this component's properties
UTestHTNComponent::UTestHTNComponent()
{
}


// Called when the game starts
void UTestHTNComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateGetFoodPlan();
}

void UTestHTNComponent::CreateGetFoodPlan()
{
	// Initialize world state properties
    UHTNWorldState* State = GetWorldState();
    
    // Add our locations to the world state
    State->SetPropertyValue<FVector>("FridgeLocation", FridgeLocation);
    State->SetPropertyValue<FVector>("TableLocation", TableLocation);
    State->SetPropertyValue<FVector>("DoorLocation", DoorLocation);
    
    // Randomly decide if the fridge has food
    bool bFridgeHasFood = FMath::RandBool();
    State->SetPropertyValue<bool>("FridgeHasFood", bFridgeHasFood);
    
    // Initialize food-related state properties
    State->SetPropertyValue<bool>("HasFood", false);
    State->SetPropertyValue<bool>("IsEating", false);
    
    // Store animation montages in the world state
    State->SetPropertyValue<UObject*>("UseFridgeAnimation", UseFridgeAnimation);
    State->SetPropertyValue<UObject*>("UseTableAnimation", UseTableAnimation);
    State->SetPropertyValue<UObject*>("UseDoorAnimation", UseDoorAnimation);
    
    // Create the main compound task for getting food
    UHTNCompoundTask* GetFoodTask = UHTNTaskFactory::Get()->CreateCompoundTask(UHTNCompoundTask::StaticClass(), this, "GetFood");
    
    // === METHOD 1: Get food from fridge ===
    UHTNMethod* FridgeMethod = NewObject<UHTNMethod>(GetFoodTask);
    FridgeMethod->MethodName = "GetFoodFromFridge";
    FridgeMethod->Description = "Get food from the fridge and eat at the table";
    FridgeMethod->Priority = 2.0f; // Higher priority than ordering food
    
    // Add fridge has food condition
    UHTNPropertyCondition* FridgeHasFoodCondition = NewObject<UHTNPropertyCondition>(FridgeMethod);
    FridgeHasFoodCondition->PropertyKey = "FridgeHasFood";
    FridgeHasFoodCondition->CheckType = EHTNPropertyCheckType::IsTrue;
    FridgeMethod->Conditions.Add(FridgeHasFoodCondition);
    
    // Tasks for getting food from fridge
    // 1. Move to fridge
    UHTNMoveToTask* MoveToFridgeTask = Cast<UHTNMoveToTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNMoveToTask::StaticClass(), GetFoodTask, "MoveToFridge"));
    MoveToFridgeTask->bUseParameterLocation = false;
    MoveToFridgeTask->DestinationPropertyKey = "FridgeLocation";
    MoveToFridgeTask->AcceptanceRadius = 100.0f;
    
    // 2. Use fridge animation
    UHTNPlayMontageTask* UseFridgeTask = Cast<UHTNPlayMontageTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPlayMontageTask::StaticClass(), GetFoodTask, "UseFridge"));
    UseFridgeTask->bUseMontageFromWorldState = true;
    UseFridgeTask->MontagePropertyKey = "UseFridgeAnimation";
    UseFridgeTask->bWaitForCompletion = true;
    
    // Effect: HasFood = true
    UHTNSetPropertyEffect* HasFoodEffect = NewObject<UHTNSetPropertyEffect>(UseFridgeTask);
    HasFoodEffect->PropertyKey = "HasFood";
    HasFoodEffect->PropertyValue = FHTNProperty(true);
    UseFridgeTask->Effects.Add(HasFoodEffect);
    
    // 3. Move to table
    UHTNMoveToTask* MoveToTableTask = Cast<UHTNMoveToTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNMoveToTask::StaticClass(), GetFoodTask, "MoveToTable"));
    MoveToTableTask->bUseParameterLocation = false;
    MoveToTableTask->DestinationPropertyKey = "TableLocation";
    MoveToTableTask->AcceptanceRadius = 100.0f;
    
    // 4. Eat at table
    UHTNPlayMontageTask* UseTableTask = Cast<UHTNPlayMontageTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPlayMontageTask::StaticClass(), GetFoodTask, "UseTable"));
    UseTableTask->bUseMontageFromWorldState = true;
    UseTableTask->MontagePropertyKey = "UseTableAnimation";
    UseTableTask->bWaitForCompletion = true;
    
    // Effect: IsEating = true
    UHTNSetPropertyEffect* IsEatingEffect = NewObject<UHTNSetPropertyEffect>(UseTableTask);
    IsEatingEffect->PropertyKey = "IsEating";
    IsEatingEffect->PropertyValue = FHTNProperty(true);
    UseTableTask->Effects.Add(IsEatingEffect);
    
    // Add tasks to the method
    FridgeMethod->Subtasks.Add(MoveToFridgeTask);
    FridgeMethod->Subtasks.Add(UseFridgeTask);
    FridgeMethod->Subtasks.Add(MoveToTableTask);
    FridgeMethod->Subtasks.Add(UseTableTask);
    
    // === METHOD 2: Order food (if fridge is empty) ===
    UHTNMethod* OrderFoodMethod = NewObject<UHTNMethod>(GetFoodTask);
    OrderFoodMethod->MethodName = "OrderFoodDelivery";
    OrderFoodMethod->Description = "Order food delivery and wait at the door";
    OrderFoodMethod->Priority = 1.0f; // Lower priority than getting food from fridge
    
    // Add fridge is empty condition
    UHTNPropertyCondition* FridgeIsEmptyCondition = NewObject<UHTNPropertyCondition>(OrderFoodMethod);
    FridgeIsEmptyCondition->PropertyKey = "FridgeHasFood";
    FridgeIsEmptyCondition->CheckType = EHTNPropertyCheckType::IsFalse;
    OrderFoodMethod->Conditions.Add(FridgeIsEmptyCondition);
    
    // Tasks for ordering food
    // 1. Print log about ordering food
    UHTNPrintLogTask* OrderFoodTask = Cast<UHTNPrintLogTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPrintLogTask::StaticClass(), GetFoodTask, "OrderFood"));
    OrderFoodTask->Message = "Fridge is empty! Ordering food from UberEats...";
    OrderFoodTask->DisplayMode = EHTNLogDisplayMode::Both;
    OrderFoodTask->DisplayDuration = 5.0f;
    OrderFoodTask->Verbosity = EHTNLogVerbosity::Info;
    
    // Effect: HasFood = true (will be true after delivery)
    UHTNSetPropertyEffect* DeliveryEffect = NewObject<UHTNSetPropertyEffect>(OrderFoodTask);
    DeliveryEffect->PropertyKey = "HasFood";
    DeliveryEffect->PropertyValue = FHTNProperty(true);
    OrderFoodTask->Effects.Add(DeliveryEffect);
    
    // 2. Move to door to wait for delivery
    UHTNMoveToTask* MoveToDoorTask = Cast<UHTNMoveToTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNMoveToTask::StaticClass(), GetFoodTask, "MoveToDoor"));
    MoveToDoorTask->bUseParameterLocation = false;
    MoveToDoorTask->DestinationPropertyKey = "DoorLocation";
    MoveToDoorTask->AcceptanceRadius = 100.0f;
    
    // 3. Answer door animation
    UHTNPlayMontageTask* UseDoorTask = Cast<UHTNPlayMontageTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPlayMontageTask::StaticClass(), GetFoodTask, "UseDoor"));
    UseDoorTask->bUseMontageFromWorldState = true;
    UseDoorTask->MontagePropertyKey = "UseDoorAnimation";
    UseDoorTask->bWaitForCompletion = true;
    
    // 4. Move to table with delivered food
    UHTNMoveToTask* MoveToTableWithDeliveryTask = Cast<UHTNMoveToTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNMoveToTask::StaticClass(), GetFoodTask, "MoveToTableWithDelivery"));
    MoveToTableWithDeliveryTask->bUseParameterLocation = false;
    MoveToTableWithDeliveryTask->DestinationPropertyKey = "TableLocation";
    MoveToTableWithDeliveryTask->AcceptanceRadius = 100.0f;
    
    // 5. Eat delivered food at table
    UHTNPlayMontageTask* EatDeliveredFoodTask = Cast<UHTNPlayMontageTask>(UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPlayMontageTask::StaticClass(), GetFoodTask, "EatDeliveredFood"));
    EatDeliveredFoodTask->bUseMontageFromWorldState = true;
    EatDeliveredFoodTask->MontagePropertyKey = "UseTableAnimation";
    EatDeliveredFoodTask->bWaitForCompletion = true;
    
    // Effect: IsEating = true
    UHTNSetPropertyEffect* IsEatingDeliveryEffect = NewObject<UHTNSetPropertyEffect>(EatDeliveredFoodTask);
    IsEatingDeliveryEffect->PropertyKey = "IsEating";
    IsEatingDeliveryEffect->PropertyValue = FHTNProperty(true);
    EatDeliveredFoodTask->Effects.Add(IsEatingDeliveryEffect);
    
    // Add tasks to the method
    OrderFoodMethod->Subtasks.Add(OrderFoodTask);
    OrderFoodMethod->Subtasks.Add(MoveToDoorTask);
    OrderFoodMethod->Subtasks.Add(UseDoorTask);
    OrderFoodMethod->Subtasks.Add(MoveToTableWithDeliveryTask);
    OrderFoodMethod->Subtasks.Add(EatDeliveredFoodTask);
    
    // Add methods to the compound task
    GetFoodTask->Methods.Add(FridgeMethod);
    GetFoodTask->Methods.Add(OrderFoodMethod);
    
    // Create a list of goal tasks
    TArray<UHTNTask*> GoalTasks;
    GoalTasks.Add(GetFoodTask);
    
    // Generate the plan
    if (GeneratePlan(GoalTasks))
    {
        UE_LOG(LogTemp, Display, TEXT("Successfully generated Get Food plan! Fridge has food: %s"), 
               bFridgeHasFood ? TEXT("Yes") : TEXT("No"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to generate Get Food plan"));
    }
}

