// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNPlanAssetEditor.h"

#include "HTNEditorLogging.h"
#include "HTNGraphData.h"
#include "HTNPlanAsset.h"
#include "HTNGraph.h"
#include "HTNGraphNode_Root.h"
#include "Framework/Commands/GenericCommands.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "ToolMenus.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenus.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "HTNPlanAssetEditor"

const FName FHTNPlanAssetEditor::GraphTabId(TEXT("HTNPlanAssetEditor_Graph"));
const FName FHTNPlanAssetEditor::PropertiesTabId(TEXT("HTNPlanAssetEditor_Properties"));
const FName FHTNPlanAssetEditor::PlanDetailsTabId(TEXT("HTNPlanAssetEditor_PlanDetails"));

void FHTNPlanAssetEditor::InitHTNPlanAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHTNPlanAsset* InPlanAsset)
{
    PlanAsset = InPlanAsset;
    
    // Create the graph data
    GraphData = CreateGraphData();
    
    // Set up commands
    GraphEditorCommands = MakeShareable(new FUICommandList);
    
    // Setup generic graph editor commands
    GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnCommandDelete),
        FCanExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::CanDeleteNode));
    
    GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnCommandCopy),
        FCanExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::CanCopyNode));
    
    GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnCommandPaste),
        FCanExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::CanPasteNode));
    
    GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnCommandCut),
        FCanExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::CanCutNode));
    
    GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnCommandDuplicate),
        FCanExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::CanDuplicateNode));
    
    // Create the graph editor widget
    CreateGraphEditorWidget();
    
    // Create the tab layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_HTNPlanAssetEditor_Layout_v1")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewStack()
                ->SetSizeCoefficient(0.1f)
                ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
                ->SetHideTabWell(true)
            )
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->AddTab(GraphTabId, ETabState::OpenedTab)
                    ->SetHideTabWell(false)
                )
                ->Split
                (
                    FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
                    ->SetSizeCoefficient(0.3f)
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.7f)
                        ->AddTab(PropertiesTabId, ETabState::OpenedTab)
                    )
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.3f)
                        ->AddTab(PlanDetailsTabId, ETabState::OpenedTab)
                    )
                )
            )
        );
    
    // Initialize the asset editor
    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = true;
    InitAssetEditor(Mode, InitToolkitHost, TEXT("HTNPlanAssetEditorApp"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, InPlanAsset);
    
    // Extend the toolbar for HTN-specific actions
    ExtendToolbar();
}

void FHTNPlanAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_HTNPlanAssetEditor", "HTN Plan Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
    
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
    
    InTabManager->RegisterTabSpawner(GraphTabId, FOnSpawnTab::CreateSP(this, &FHTNPlanAssetEditor::SpawnTab_GraphCanvas))
        .SetDisplayName(LOCTEXT("GraphTab", "Graph"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));
    
    InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FHTNPlanAssetEditor::SpawnTab_Properties))
        .SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
    
    InTabManager->RegisterTabSpawner(PlanDetailsTabId, FOnSpawnTab::CreateSP(this, &FHTNPlanAssetEditor::SpawnTab_PlanDetails))
        .SetDisplayName(LOCTEXT("PlanDetailsTab", "Plan"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FHTNPlanAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    InTabManager->UnregisterTabSpawner(GraphTabId);
    InTabManager->UnregisterTabSpawner(PropertiesTabId);
    InTabManager->UnregisterTabSpawner(PlanDetailsTabId);
    
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FName FHTNPlanAssetEditor::GetToolkitFName() const
{
    return FName("HTNPlanAssetEditor");
}

FText FHTNPlanAssetEditor::GetBaseToolkitName() const
{
    return LOCTEXT("HTNPlanAssetEditorToolkit", "HTN Plan Editor");
}

FString FHTNPlanAssetEditor::GetWorldCentricTabPrefix() const
{
    return TEXT("HTNPlanAssetEditor ");
}

FLinearColor FHTNPlanAssetEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.3f, 0.6f, 1.0f); // Light blue
}

bool FHTNPlanAssetEditor::OnRequestClose()
{
    // Compile the graph before closing to ensure changes are saved
    CompileHTNPlan();
    
    return FAssetEditorToolkit::OnRequestClose();
}

bool FHTNPlanAssetEditor::CompileHTNPlan()
{
    if (!GraphData || !PlanAsset)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot compile HTN plan: Missing graph data or plan asset"));
        return false;
    }
    
    // Compile the graph to the plan asset
    bool bSuccess = GraphData->CompileGraphToAsset();
    
    if (bSuccess)
    {
        // Mark the package as dirty
        PlanAsset->MarkPackageDirty();
        
        // Notify any listeners that the plan has been updated
        FAssetRegistryModule::AssetCreated(PlanAsset);
        
        UE_LOG(LogHTNPlannerEditorPlugin, Log, TEXT("Successfully compiled HTN plan"));
    }
    else
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Failed to compile HTN plan"));
    }
    
    return bSuccess;
}

bool FHTNPlanAssetEditor::ValidateHTNPlan()
{
    if (!GraphData)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot validate HTN plan: Missing graph data"));
        return false;
    }
    
    // Validate the graph
    TArray<FString> ValidationErrors;
    bool bIsValid = GraphData->ValidateGraph(ValidationErrors);
    
    // Display the validation results
    if (bIsValid)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Log, TEXT("HTN plan validation successful"));
    }
    else
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("HTN plan validation failed:"));
        for (const FString& Error : ValidationErrors)
        {
            UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("  %s"), *Error);
        }
    }
    
    return bIsValid;
}

UEdGraphNode* FHTNPlanAssetEditor::AddNewNode(TSubclassOf<UEdGraphNode> NodeClass, const FVector2D& Location, bool bSelectNewNode)
{
    if (!GraphData || !GraphData->GetGraph())
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot add node: Missing graph"));
        return nullptr;
    }
    
    // Create the node
    UEdGraphNode* NewNode = NewObject<UEdGraphNode>(GraphData->GetGraph(), NodeClass);
    
    // Set its position
    NewNode->NodePosX = Location.X;
    NewNode->NodePosY = Location.Y;
    
    // Add it to the graph
    GraphData->GetGraph()->AddNode(NewNode, true, bSelectNewNode);
    
    // Create default pins
    NewNode->AllocateDefaultPins();
    
    return NewNode;
}

void FHTNPlanAssetEditor::CreateGraphEditorWidget()
{
    if (!GraphData || !GraphData->GetGraph())
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot create graph editor: Missing graph"));
        return;
    }
    
    // Create the graph editor
    SGraphEditor::FGraphEditorEvents InEvents;
    InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FHTNPlanAssetEditor::OnSelectedNodesChanged);
    
    GraphEditorWidget = SNew(SGraphEditor)
        .AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
        .GraphToEdit(GraphData->GetGraph())
		.GraphEvents(InEvents);
}

void FHTNPlanAssetEditor::OnGraphChanged(const FEdGraphEditAction& Action)
{
    // Compile the graph when it changes (could add debouncing for performance)
    CompileHTNPlan();
}

void FHTNPlanAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& SelectedNodes)
{
    // Update property panel with selected node details
    TArray<UObject*> SelectedNodeArray;
    for (UObject* SelectedNode : SelectedNodes)
    {
        SelectedNodeArray.Add(SelectedNode);
    }
    
    // Update property view with selected nodes
    if (DetailsView.IsValid())
    {
        DetailsView->SetObjects(SelectedNodeArray);
    }
}

UHTNGraphData* FHTNPlanAssetEditor::CreateGraphData()
{
    if (!PlanAsset)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot create graph data: No plan asset"));
        return nullptr;
    }
    
    // Create the graph data object
    UHTNGraphData* NewGraphData = NewObject<UHTNGraphData>(GetTransientPackage(), UHTNGraphData::StaticClass());
    
    // Initialize the graph data
    NewGraphData->Initialize(PlanAsset);
    
    // Create or update the graph
    NewGraphData->UpdateGraphFromAsset();
    
    return NewGraphData;
}

TSharedRef<SDockTab> FHTNPlanAssetEditor::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == GraphTabId);
    
    TSharedRef<SDockTab> GraphTab = SNew(SDockTab)
        .Label(LOCTEXT("HTNGraph", "HTN Graph"))
        [
            GraphEditorWidget.ToSharedRef()
        ];
    
    return GraphTab;
}

TSharedRef<SDockTab> FHTNPlanAssetEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PropertiesTabId);
    
    // Create the details view if it doesn't exist
    if (!DetailsView.IsValid())
    {
        FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        
        FDetailsViewArgs DetailsViewArgs;
        DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        DetailsViewArgs.bHideSelectionTip = true;
        
        DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
        DetailsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateSP(this, &FHTNPlanAssetEditor::IsPropertyVisible));
        
        // Initialize with empty selection
        DetailsView->SetObjects(TArray<UObject*>());
    }
    
    TSharedRef<SDockTab> PropertiesTab = SNew(SDockTab)
        .Label(LOCTEXT("NodeProperties", "Properties"))
        [
            DetailsView.ToSharedRef()
        ];
    
    return PropertiesTab;
}

TSharedRef<SDockTab> FHTNPlanAssetEditor::SpawnTab_PlanDetails(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PlanDetailsTabId);
    
    // Create the plan details view if it doesn't exist
    if (!PlanDetailsView.IsValid())
    {
        FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        
        FDetailsViewArgs DetailsViewArgs;
        DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
        DetailsViewArgs.bHideSelectionTip = true;
        
        PlanDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
        
        // Set the plan asset as the object to show
        if (PlanAsset)
        {
            TArray<UObject*> Objects;
            Objects.Add(PlanAsset);
            PlanDetailsView->SetObjects(Objects);
        }
    }
    
    TSharedRef<SDockTab> PlanDetailsTab = SNew(SDockTab)
        .Label(LOCTEXT("PlanDetails", "Plan Details"))
        [
            PlanDetailsView.ToSharedRef()
        ];
    
    return PlanDetailsTab;
}

void FHTNPlanAssetEditor::ExtendToolbar()
{
    struct Local
    {
        static void FillToolbar(FToolBarBuilder& ToolbarBuilder, FHTNPlanAssetEditor* Editor)
        {
            ToolbarBuilder.BeginSection("HTNPlan");
            {
                ToolbarBuilder.AddToolBarButton(
                    FUIAction(
                        FExecuteAction::CreateLambda([Editor]() { Editor->CompileHTNPlan(); }),
                        FCanExecuteAction::CreateLambda([]() { return true; })
                    ),
                    NAME_None,
                    LOCTEXT("CompilePlan", "Compile"),
                    LOCTEXT("CompilePlanTooltip", "Compile the HTN graph to a plan asset"),
                    FSlateIcon(FAppStyle::GetAppStyleSetName(), "AssetEditor.Apply")
                );
                
                ToolbarBuilder.AddToolBarButton(
                    FUIAction(
                        FExecuteAction::CreateLambda([Editor]() { Editor->ValidateHTNPlan(); }),
                        FCanExecuteAction::CreateLambda([]() { return true; })
                    ),
                    NAME_None,
                    LOCTEXT("ValidatePlan", "Validate"),
                    LOCTEXT("ValidatePlanTooltip", "Validate the HTN graph"),
                    FSlateIcon(FAppStyle::GetAppStyleSetName(), "AssetEditor.CheckForErrors")
                );
            }
            ToolbarBuilder.EndSection();
        }
    };
    
    TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    
    ToolbarExtender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar, this)
    );
    
    AddToolbarExtender(ToolbarExtender);
}

bool FHTNPlanAssetEditor::IsPropertyVisible(const FPropertyAndParent& PropertyAndParent)
{
    // Filter to show only editor properties in the node details panel
    for(TWeakObjectPtr<UObject> Object : PropertyAndParent.Objects)
    {
        if (Cast<UHTNGraphNode>(Object))
        {
            // Hide runtime-only properties that shouldn't be edited directly
            if (PropertyAndParent.Property.GetName().StartsWith("Runtime"))
            {
                return false;
            }
        }
    }
    return true;
}

void FHTNPlanAssetEditor::OnCommandDelete()
{
    // Execute the delete command on our command list
    GraphEditorCommands->ExecuteAction(FGenericCommands::Get().Delete.ToSharedRef());
}

bool FHTNPlanAssetEditor::CanDeleteNode() const
{
    if (GraphEditorWidget.IsValid())
    {
        TSet<UObject*> SelectedNodes = GraphEditorWidget->GetSelectedNodes();
        
        // Make sure we're not trying to delete the root node
        for (UObject* Node : SelectedNodes)
        {
            if (UHTNGraphNode_Root* RootNode = Cast<UHTNGraphNode_Root>(Node))
            {
                // Can't delete the root node
                return false;
            }
        }
        
        return SelectedNodes.Num() > 0;
    }
    return false;
}

void FHTNPlanAssetEditor::OnCommandCopy()
{
    // Execute the copy command on our command list
    GraphEditorCommands->ExecuteAction(FGenericCommands::Get().Copy.ToSharedRef());
}

bool FHTNPlanAssetEditor::CanCopyNode() const
{
    if (GraphEditorWidget.IsValid())
    {
        TSet<UObject*> SelectedNodes = GraphEditorWidget->GetSelectedNodes();
        return SelectedNodes.Num() > 0;
    }
    return false;
}

void FHTNPlanAssetEditor::OnCommandPaste()
{
    // Execute the paste command on our command list
    GraphEditorCommands->ExecuteAction(FGenericCommands::Get().Paste.ToSharedRef());
}

bool FHTNPlanAssetEditor::CanPasteNode() const
{
    if (GraphEditorWidget.IsValid())
    {
        // Let the graph editor determine if paste is possible
        return true;
    }
    return false;
}

void FHTNPlanAssetEditor::OnCommandCut()
{
    // Execute the cut command on our command list  
    GraphEditorCommands->ExecuteAction(FGenericCommands::Get().Cut.ToSharedRef());
}

bool FHTNPlanAssetEditor::CanCutNode() const
{
    if (GraphEditorWidget.IsValid())
    {
        TSet<UObject*> SelectedNodes = GraphEditorWidget->GetSelectedNodes();
        
        // Make sure we're not trying to cut the root node
        for (UObject* Node : SelectedNodes)
        {
            if (UHTNGraphNode_Root* RootNode = Cast<UHTNGraphNode_Root>(Node))
            {
                // Can't cut the root node
                return false;
            }
        }
        
        return SelectedNodes.Num() > 0;
    }
    return false;
}

void FHTNPlanAssetEditor::OnCommandDuplicate()
{
    // Execute the duplicate command on our command list
    GraphEditorCommands->ExecuteAction(FGenericCommands::Get().Duplicate.ToSharedRef());
}

bool FHTNPlanAssetEditor::CanDuplicateNode() const
{
    if (GraphEditorWidget.IsValid())
    {
        TSet<UObject*> SelectedNodes = GraphEditorWidget->GetSelectedNodes();
        
        // Make sure we're not trying to duplicate the root node
        for (UObject* Node : SelectedNodes)
        {
            if (UHTNGraphNode_Root* RootNode = Cast<UHTNGraphNode_Root>(Node))
            {
                // Can't duplicate the root node
                return false;
            }
        }
        
        return SelectedNodes.Num() > 0;
    }
    return false;
}