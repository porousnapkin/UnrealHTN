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
#include "HTNGraphEditorCommands.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HTNGraphNode_PrimitiveTask.h"
#include "HTNGraphNode_CompoundTask.h"
#include "HTNGraphNode_Method.h"
#include "HTNGraphNode_Condition.h"
#include "HTNGraphNode_Effect.h"

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

    // Map HTN-specific commands
    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AddPrimitiveTaskNode,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAddPrimitiveTaskNode),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AddCompoundTaskNode,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAddCompoundTaskNode),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AddMethodNode,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAddMethodNode),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AddConditionNode,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAddConditionNode),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AddEffectNode,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAddEffectNode),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().AutoArrangeNodes,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnAutoArrangeNodes),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().FocusOnSelection,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnFocusOnSelection),
        FCanExecuteAction::CreateLambda([]() { return true; }));

    GraphEditorCommands->MapAction(FHTNGraphEditorCommands::Get().ToggleDebugView,
        FExecuteAction::CreateSP(this, &FHTNPlanAssetEditor::OnToggleDebugView),
        FCanExecuteAction::CreateLambda([]() { return true; }));
    
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

                // Add a separator for node creation commands
                ToolbarBuilder.AddSeparator();

                // Add node creation buttons using your registered commands
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AddPrimitiveTaskNode);
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AddCompoundTaskNode);
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AddMethodNode);
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AddConditionNode);
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AddEffectNode);

                // Add another separator for organization commands
                ToolbarBuilder.AddSeparator();

                // Add graph organization buttons
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().AutoArrangeNodes);
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().FocusOnSelection);

                // Add debug button
                ToolbarBuilder.AddToolBarButton(FHTNGraphEditorCommands::Get().ToggleDebugView);
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

// Node creation functions
void FHTNPlanAssetEditor::OnAddPrimitiveTaskNode()
{
    // Get cursor location in the graph
    FVector2D NodePosition = GraphEditorWidget->GetPasteLocation();

    // Create the primitive task node at the cursor position
    UHTNGraphNode_PrimitiveTask* NewNode = Cast<UHTNGraphNode_PrimitiveTask>(
        AddNewNode(UHTNGraphNode_PrimitiveTask::StaticClass(), NodePosition, true));

    if (NewNode)
    {
        // Initialize with default values if needed
        NewNode->NodeTitle = LOCTEXT("NewPrimitiveTask", "New Primitive Task");

        // Notify changes
        GraphData->GetGraph()->NotifyGraphChanged();

        // Focus on the newly created node
        GraphEditorWidget->SetNodeSelection(NewNode, true);
    }
}

void FHTNPlanAssetEditor::OnAddCompoundTaskNode()
{
    FVector2D NodePosition = GraphEditorWidget->GetPasteLocation();

    UHTNGraphNode_CompoundTask* NewNode = Cast<UHTNGraphNode_CompoundTask>(
        AddNewNode(UHTNGraphNode_CompoundTask::StaticClass(), NodePosition, true));

    if (NewNode)
    {
        NewNode->NodeTitle = LOCTEXT("NewCompoundTask", "New Compound Task");
        GraphData->GetGraph()->NotifyGraphChanged();
        GraphEditorWidget->SetNodeSelection(NewNode, true);
    }
}

void FHTNPlanAssetEditor::OnAddMethodNode()
{
    FVector2D NodePosition = GraphEditorWidget->GetPasteLocation();

    UHTNGraphNode_Method* NewNode = Cast<UHTNGraphNode_Method>(
        AddNewNode(UHTNGraphNode_Method::StaticClass(), NodePosition, true));

    if (NewNode)
    {
        NewNode->NodeTitle = LOCTEXT("NewMethod", "New Method");
        GraphData->GetGraph()->NotifyGraphChanged();
        GraphEditorWidget->SetNodeSelection(NewNode, true);
    }
}

void FHTNPlanAssetEditor::OnAddConditionNode()
{
    FVector2D NodePosition = GraphEditorWidget->GetPasteLocation();

    UHTNGraphNode_Condition* NewNode = Cast<UHTNGraphNode_Condition>(
        AddNewNode(UHTNGraphNode_Condition::StaticClass(), NodePosition, true));

    if (NewNode)
    {
        NewNode->NodeTitle = LOCTEXT("NewCondition", "New Condition");
        GraphData->GetGraph()->NotifyGraphChanged();
        GraphEditorWidget->SetNodeSelection(NewNode, true);
    }
}

void FHTNPlanAssetEditor::OnAddEffectNode()
{
    FVector2D NodePosition = GraphEditorWidget->GetPasteLocation();

    UHTNGraphNode_Effect* NewNode = Cast<UHTNGraphNode_Effect>(
        AddNewNode(UHTNGraphNode_Effect::StaticClass(), NodePosition, true));

    if (NewNode)
    {
        NewNode->NodeTitle = LOCTEXT("NewEffect", "New Effect");
        GraphData->GetGraph()->NotifyGraphChanged();
        GraphEditorWidget->SetNodeSelection(NewNode, true);
    }
}

// Graph organization functions
void FHTNPlanAssetEditor::OnAutoArrangeNodes()
{
    if (!GraphEditorWidget.IsValid() || !GraphData || !GraphData->GetGraph())
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Warning, TEXT("Cannot auto-arrange nodes: Invalid graph editor or graph"));
        return;
    }

    // Get the graph
    UEdGraph* Graph = GraphData->GetGraph();

    // Constants for layout
    const float HorizontalSpacing = 200.0f;
    const float VerticalSpacing = 150.0f;
    const float StartX = 0.0f;
    const float StartY = 0.0f;

    // Find the root node
    UHTNGraphNode_Root* RootNode = nullptr;
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (UHTNGraphNode_Root* TestRoot = Cast<UHTNGraphNode_Root>(Node))
        {
            RootNode = TestRoot;
            break;
        }
    }

    if (RootNode)
    {
        // Set the root position
        RootNode->NodePosX = StartX;
        RootNode->NodePosY = StartY;

        // Simple layered layout algorithm
        TMap<int32, TArray<UEdGraphNode*>> Layers;

        // Start with the root at layer 0
        Layers.Add(0, { RootNode });

        // Determine layer for each node by traversing from the root
        TArray<UEdGraphNode*> NodesToProcess = { RootNode };
        TSet<UEdGraphNode*> ProcessedNodes;

        while (NodesToProcess.Num() > 0)
        {
            UEdGraphNode* CurrentNode = NodesToProcess[0];
            NodesToProcess.RemoveAt(0);

            if (ProcessedNodes.Contains(CurrentNode))
            {
                continue;
            }

            ProcessedNodes.Add(CurrentNode);

            // Find the layer of the current node
            int32 CurrentLayer = -1;
            for (const auto& LayerEntry : Layers)
            {
                if (LayerEntry.Value.Contains(CurrentNode))
                {
                    CurrentLayer = LayerEntry.Key;
                    break;
                }
            }

            if (CurrentLayer == -1)
            {
                continue;
            }

            // Process output pins and their connections
            for (UEdGraphPin* Pin : CurrentNode->Pins)
            {
                if (Pin->Direction == EGPD_Output)
                {
                    for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
                    {
                        UEdGraphNode* TargetNode = LinkedPin->GetOwningNode();

                        if (!ProcessedNodes.Contains(TargetNode))
                        {
                            // Add to the next layer
                            int32 NextLayer = CurrentLayer + 1;
                            if (!Layers.Contains(NextLayer))
                            {
                                Layers.Add(NextLayer, {});
                            }
                            Layers[NextLayer].Add(TargetNode);
                            NodesToProcess.Add(TargetNode);
                        }
                    }
                }
            }
        }

        // Position nodes by layer
        for (const auto& LayerEntry : Layers)
        {
            int32 Layer = LayerEntry.Key;
            const TArray<UEdGraphNode*>& NodesInLayer = LayerEntry.Value;

            float XPos = StartX;
            float YPos = StartY + Layer * VerticalSpacing;

            // Position each node in this layer
            for (int32 i = 0; i < NodesInLayer.Num(); ++i)
            {
                UEdGraphNode* Node = NodesInLayer[i];
                Node->NodePosX = XPos + (i * HorizontalSpacing);
                Node->NodePosY = YPos;
            }
        }

        // Notify that the graph has changed to update the view
        Graph->NotifyGraphChanged();
    }
    else
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Warning, TEXT("Cannot auto-arrange nodes: No root node found"));
    }
}

void FHTNPlanAssetEditor::OnFocusOnSelection()
{
    if (GraphEditorWidget.IsValid())
    {
        // Focus the graph editor on the selected nodes
        GraphEditorWidget->ZoomToFit(true); // true = only selected nodes
    }
}

void FHTNPlanAssetEditor::OnToggleDebugView()
{
    if (!GraphData)
    {
        return;
    }

    // Toggle the debug view state
    GraphData->bIsInDebugMode = !GraphData->bIsInDebugMode;

    // Update the graph display based on the debug state
    if (GraphData->bIsInDebugMode)
    {
        // Enter debug visualization mode
        UE_LOG(LogHTNPlannerEditorPlugin, Log, TEXT("Debug view enabled"));

        // If you have additional debug visualization setup, add it here
        // For example, you might update node appearances or display execution statistics
    }
    else
    {
        // Exit debug visualization mode
        UE_LOG(LogHTNPlannerEditorPlugin, Log, TEXT("Debug view disabled"));

        // Reset any debug visualization
    }

    // Refresh the graph to show debug information
    if (GraphEditorWidget.IsValid())
    {
        GraphEditorWidget->NotifyGraphChanged();
    }
}