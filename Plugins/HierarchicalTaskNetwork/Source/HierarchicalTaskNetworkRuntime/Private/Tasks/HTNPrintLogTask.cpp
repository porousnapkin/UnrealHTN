#include "Tasks/HTNPrintLogTask.h"
#include "HTNExecutionContext.h"
#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"

UHTNPrintLogTask::UHTNPrintLogTask()
    : Message(TEXT("Debug Message"))
    , bIncludeWorldStateValues(true)
    , DisplayMode(EHTNLogDisplayMode::Both)
    , Verbosity(EHTNLogVerbosity::Info)
    , DisplayDuration(2.0f)
    , DisplayColor(FLinearColor::White)
{
    TaskName = FName("PrintLog");
    DebugColor = FLinearColor(0.5f, 0.5f, 0.5f); // Gray for debug tasks
}

EHTNTaskStatus UHTNPrintLogTask::ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext)
{
    if (!ExecutionContext || !ExecutionContext->GetWorldState())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PrintLogTask: No execution context or world state"));
        return EHTNTaskStatus::Failed;
    }
    
    // Process the message text, replacing tokens with world state values if enabled
    FString FormattedMessage = ProcessMessageText(ExecutionContext->GetWorldState());
    
    // Display the message
    DisplayMessage(FormattedMessage);
    
    // This task always succeeds immediately
    return EHTNTaskStatus::Succeeded;
}

bool UHTNPrintLogTask::ValidateTask_Implementation() const
{
    // First validate the base task
    if (!Super::ValidateTask_Implementation())
    {
        return false;
    }
    
    // Simple validation - just make sure the message isn't empty
    if (Message.IsEmpty())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PrintLogTask: Message is empty"));
        return false;
    }
    
    return true;
}

FString UHTNPrintLogTask::ProcessMessageText(const UHTNWorldState* WorldState) const
{
    FString FormattedMessage = Message;
    
    // If we don't need to include world state values, just return the raw message
    if (!bIncludeWorldStateValues || !WorldState)
    {
        return FormattedMessage;
    }
    
    // Get all property names from the world state
    TArray<FName> PropertyNames = WorldState->GetPropertyNames();
    
    // For each property, replace {PropertyName} tokens with the actual value
    for (const FName& PropertyName : PropertyNames)
    {
        FString TokenPattern = FString::Printf(TEXT("{%s}"), *PropertyName.ToString());
        
        // Check if the token exists in the message
        if (FormattedMessage.Contains(TokenPattern))
        {
            // Get the property value
            FHTNProperty PropertyValue;
            if (WorldState->GetProperty(PropertyName, PropertyValue))
            {
                // Convert the property value to string and replace the token
                FString ValueStr = PropertyValue.ToString();
                FormattedMessage = FormattedMessage.Replace(*TokenPattern, *ValueStr);
            }
        }
    }
    
    return FormattedMessage;
}

void UHTNPrintLogTask::DisplayMessage(const FString& FormattedMessage) const
{
    // Determine the log verbosity based on the configured verbosity
    ELogVerbosity::Type LogVerbosity;
    FColor ScreenColor;
    
    switch (Verbosity)
    {
        case EHTNLogVerbosity::Warning:
            LogVerbosity = ELogVerbosity::Warning;
            ScreenColor = FColor::Yellow;
            break;
        
        case EHTNLogVerbosity::Error:
            LogVerbosity = ELogVerbosity::Error;
            ScreenColor = FColor::Red;
            break;
        
        case EHTNLogVerbosity::Info:
        default:
            LogVerbosity = ELogVerbosity::Display;
            ScreenColor = DisplayColor.ToFColor(true);
            break;
    }
    
    // Format the message with a prefix for better identification
    FString PrefixedMessage = FString::Printf(TEXT("[HTN Debug] %s"), *FormattedMessage);
    
    // Display the message according to the configured display mode
    switch (DisplayMode)
    {
        case EHTNLogDisplayMode::Screen:
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, DisplayDuration, ScreenColor, PrefixedMessage);
            }
            break;
        
        case EHTNLogDisplayMode::Log:
            FMsg::Logf(__FILE__, __LINE__, LogHTNPlannerPlugin.GetCategoryName(), LogVerbosity, TEXT("%s"), *PrefixedMessage);
            break;
        
        case EHTNLogDisplayMode::Both:
            FMsg::Logf(__FILE__, __LINE__, LogHTNPlannerPlugin.GetCategoryName(), LogVerbosity, TEXT("%s"), *PrefixedMessage);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, DisplayDuration, ScreenColor, PrefixedMessage);
            }
            break;
    }
}