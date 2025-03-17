// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetTypeActions_HTNGraph.h"
#include "HierarchicalTaskNetworkEditor.h"
#include "HTNPlanAsset.h"
#include <HTNPlanAssetEditor.h>

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_HTNGraph::GetName() const
{
	return LOCTEXT("HTNGraphAssetName", "HTN Graph");
}

FColor FAssetTypeActions_HTNGraph::GetTypeColor() const
{
	return FColor(63, 126, 255); // Light blue color
}

UClass* FAssetTypeActions_HTNGraph::GetSupportedClass() const
{
	return UHTNPlanAsset::StaticClass();
}

uint32 FAssetTypeActions_HTNGraph::GetCategories()
{
	return FHierarchicalTaskNetworkEditorModule::Get().GetAssetCategory();
}

void FAssetTypeActions_HTNGraph::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// For each selected asset...
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		// Get the asset
		auto HTNAsset = Cast<UHTNPlanAsset>(*ObjIt);
		if (HTNAsset)
		{
			// Create a new HTN Plan Editor
			TSharedRef<FHTNPlanAssetEditor> NewHTNEditor(new FHTNPlanAssetEditor());

			// Initialize the editor with the HTN asset
			NewHTNEditor->InitHTNPlanAssetEditor(EToolkitMode::Standalone, EditWithinLevelEditor, HTNAsset);
		}
	}
}


bool FAssetTypeActions_HTNGraph::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

void FAssetTypeActions_HTNGraph::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	auto HTNAssets = GetTypedWeakObjectPtrs<UHTNPlanAsset>(InObjects);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("HTNGraph_Validate", "Validate Plan"),
		LOCTEXT("HTNGraph_ValidateTooltip", "Validates the HTN plan structure for errors."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([=]
			{
				for (auto& Asset : HTNAssets)
				{
					if (Asset.IsValid())
					{
						// TODO: Implement validation functionality
						//UE_LOG(LogTemp, Display, TEXT("Validating HTN Graph Asset: %s"), *Asset->GetName());
					}
				}
			}),
			FCanExecuteAction::CreateLambda([=] 
			{
				for (auto& Asset : HTNAssets)
				{
					if (Asset.IsValid())
					{
						return true;
					}
				}
				return false;
			})
		)
	);
}

#undef LOCTEXT_NAMESPACE