// HTNGraphEditorCommands.h
#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "HTNGraphEditor"

class FHTNGraphEditorCommands : public TCommands<FHTNGraphEditorCommands>
{
public:
    FHTNGraphEditorCommands()
        : TCommands<FHTNGraphEditorCommands>(
            TEXT("HTNGraphEditor"), // Context name for fast lookup
            NSLOCTEXT("Contexts", "HTNGraphEditor", "HTN Graph Editor"), // Localized context name for displaying
            NAME_None, // Parent context name
            FAppStyle::GetAppStyleSetName() // Icon Style Set
        )
    {
    }

    // Register UI commands for the HTN Graph Editor
    virtual void RegisterCommands() override
    {
        UI_COMMAND(CompileHTNGraph, "Compile", "Compile the HTN graph into a runtime plan", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(ValidateHTNGraph, "Validate", "Check the HTN graph for errors", EUserInterfaceActionType::Button, FInputChord());

        // Node creation commands
        UI_COMMAND(AddPrimitiveTaskNode, "Add Primitive Task", "Add a new primitive task node to the graph", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(AddCompoundTaskNode, "Add Compound Task", "Add a new compound task node to the graph", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(AddMethodNode, "Add Method", "Add a new method node to the graph", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(AddConditionNode, "Add Condition", "Add a new condition node to the graph", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(AddEffectNode, "Add Effect", "Add a new effect node to the graph", EUserInterfaceActionType::Button, FInputChord());

        // Graph organization commands
        UI_COMMAND(AutoArrangeNodes, "Auto Arrange", "Automatically arrange the nodes in the graph", EUserInterfaceActionType::Button, FInputChord());
        UI_COMMAND(FocusOnSelection, "Focus Selection", "Center the view on the selected nodes", EUserInterfaceActionType::Button, FInputChord());

        // Debug commands
        UI_COMMAND(ToggleDebugView, "Debug View", "Toggle debug visualization for the HTN graph", EUserInterfaceActionType::ToggleButton, FInputChord());
    }

    // Define command handles
    TSharedPtr<FUICommandInfo> CompileHTNGraph;
    TSharedPtr<FUICommandInfo> ValidateHTNGraph;

    TSharedPtr<FUICommandInfo> AddPrimitiveTaskNode;
    TSharedPtr<FUICommandInfo> AddCompoundTaskNode;
    TSharedPtr<FUICommandInfo> AddMethodNode;
    TSharedPtr<FUICommandInfo> AddConditionNode;
    TSharedPtr<FUICommandInfo> AddEffectNode;

    TSharedPtr<FUICommandInfo> AutoArrangeNodes;
    TSharedPtr<FUICommandInfo> FocusOnSelection;

    TSharedPtr<FUICommandInfo> ToggleDebugView;
};

#undef LOCTEXT_NAMESPACE
