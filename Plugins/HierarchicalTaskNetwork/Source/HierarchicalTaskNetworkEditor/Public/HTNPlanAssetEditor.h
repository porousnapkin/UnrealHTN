// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class UHTNPlanAsset;
class UHTNGraphData;
class SGraphEditor;

/**
 * Asset editor for HTN Plan assets.
 * This editor provides the graph editing interface for HTN plans.
 */
class HIERARCHICALTASKNETWORKEDITOR_API FHTNPlanAssetEditor : public FAssetEditorToolkit
{
public:
    /**
     * Initialize a new instance of the editor
     */
    void InitHTNPlanAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UHTNPlanAsset* InPlanAsset);

    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual bool OnRequestClose() override;
    // End of FAssetEditorToolkit interface

    /**
     * Get the graph editor widget
     * @return The graph editor widget
     */
    TSharedRef<SGraphEditor> GetGraphEditor() const { return GraphEditorWidget.ToSharedRef(); }

    /**
     * Compile the current graph into the plan asset
     * @return True if compilation was successful
     */
    bool CompileHTNPlan();

    /**
     * Validate the current graph
     * @return True if validation passed
     */
    bool ValidateHTNPlan();

    /**
     * Add a new node to the graph
     * @param NodeClass - The class of node to add
     * @param Location - The location to place the node
     * @param bSelectNewNode - Whether to select the new node
     * @return The created node
     */
    UEdGraphNode* AddNewNode(TSubclassOf<UEdGraphNode> NodeClass, const FVector2D& Location, bool bSelectNewNode = true);

private:
    /**
     * Create the graph editor widget
     */
    void CreateGraphEditorWidget();

    /**
     * Handle graph changes
     * @param GraphEditor - The graph editor that changed
     */
    void OnGraphChanged(const FEdGraphEditAction& Action);

    /**
     * Handle node selection changes
     * @param SelectedNodes - The newly selected nodes
     */
    void OnSelectedNodesChanged(const TSet<UObject*>& SelectedNodes);

    /**
     * Create the graph data object for the asset
     * @return The graph data object
     */
    UHTNGraphData* CreateGraphData();

    // Node creation command handlers
    void OnAddPrimitiveTaskNode();
    void OnAddCompoundTaskNode();
    void OnAddMethodNode();
    void OnAddConditionNode();
    void OnAddEffectNode();

    // Graph organization command handlers
    void OnAutoArrangeNodes();
    void OnFocusOnSelection();

    // Debug command handlers
    void OnToggleDebugView();


private:
    /** Tabs IDs for the asset editor */
    static const FName GraphTabId;
    static const FName PropertiesTabId;
    static const FName PlanDetailsTabId;

    /** Plan asset being edited */
    UHTNPlanAsset* PlanAsset;

    /** Editor-only graph data */
    UHTNGraphData* GraphData;

    /** Graph editor widget */
    TSharedPtr<SGraphEditor> GraphEditorWidget;

    /** Commands for the editor */
    TSharedPtr<FUICommandList> GraphEditorCommands;

    /** Spawn the graph editor tab */
    TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);

    /** Spawn the properties tab */
    TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);

    /** Spawn the plan details tab */
    TSharedRef<SDockTab> SpawnTab_PlanDetails(const FSpawnTabArgs& Args);

    /** Build the editor toolbar */
    void ExtendToolbar();
    bool IsPropertyVisible(const FPropertyAndParent& PropertyAndParent);
    void OnCommandDelete();
    bool CanDeleteNode() const;
    void OnCommandCopy();
    bool CanCopyNode() const;
    void OnCommandPaste();
    bool CanPasteNode() const;
    void OnCommandCut();
    bool CanCutNode() const;
    void OnCommandDuplicate();
    bool CanDuplicateNode() const;

    /** Create the toolbar extensions */
    void FillToolbar(FToolBarBuilder& ToolbarBuilder);

    /** Details view for selected nodes */
    TSharedPtr<class IDetailsView> DetailsView;

    /** Details view for the plan asset */
    TSharedPtr<class IDetailsView> PlanDetailsView;
};