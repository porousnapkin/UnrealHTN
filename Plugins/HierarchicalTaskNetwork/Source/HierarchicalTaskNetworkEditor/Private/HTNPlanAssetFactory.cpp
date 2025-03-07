#include "HTNPlanAssetFactory.h"
#include "HTNPlanAsset.h"
#include "HTNGraph.h"
#include "HTNGraphNode_Root.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"

UHTNPlanAssetFactory::UHTNPlanAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UHTNPlanAsset::StaticClass();
}

UObject* UHTNPlanAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UHTNPlanAsset* NewAsset = NewObject<UHTNPlanAsset>(InParent, Class, Name, Flags);
	if (NewAsset)
	{
		// Initialize with empty default plan
		NewAsset->Description = TEXT("New HTN Plan Template");
        
		// Create the graph
		UHTNGraph* NewGraph = NewObject<UHTNGraph>(NewAsset, UHTNGraph::StaticClass(), NAME_None, RF_Transactional);
        
		// Initialize the graph with this plan asset
		NewGraph->Initialize(NewAsset);
        
		// Create a root node for the graph
		UHTNGraphNode_Root* RootNode = NewObject<UHTNGraphNode_Root>(NewGraph);
		RootNode->SetPlanAsset(NewAsset);
		NewGraph->AddNode(RootNode, /*bUserAction=*/false, /*bSelectNewNode=*/false);
        
		// Set node position
		RootNode->NodePosX = 0;
		RootNode->NodePosY = 0;
        
		// Create root node pins
		RootNode->AllocateDefaultPins();
        
		// Update the asset's modified time
		NewAsset->UpdateModifiedTime();
	}
    
	return NewAsset;
}

bool UHTNPlanAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}