// Fill out your copyright notice in the Description page of Project Settings.

#include "HierarchicalTaskNetworkEditor.h"
#include "AssetTypeActions_HTNGraph.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FHTNGraphEditor"

void FHierarchicalTaskNetworkEditorModule::StartupModule()
{
	// Register the asset category
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	HTNAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("HTN")), LOCTEXT("HTNAssetCategory", "Hierarchical Task Network"));
	
	// Register asset types
	RegisterAssetTypes();
}

void FHierarchicalTaskNetworkEditorModule::ShutdownModule()
{
	// Unregister asset types
	UnregisterAssetTypes();
}

void FHierarchicalTaskNetworkEditorModule::RegisterAssetTypes()
{
	// Load the asset tools module to register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Register HTN Graph asset type actions
	TSharedRef<IAssetTypeActions> HTNGraphAction = MakeShareable(new FAssetTypeActions_HTNGraph);
	RegisterAssetTypeActions(HTNGraphAction);
}

void FHierarchicalTaskNetworkEditorModule::UnregisterAssetTypes()
{
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 i = 0; i < CreatedAssetTypeActions.Num(); ++i)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[i].ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

void FHierarchicalTaskNetworkEditorModule::RegisterAssetTypeActions(TSharedRef<IAssetTypeActions> Action)
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHierarchicalTaskNetworkEditorModule, HierarchicalTaskNetworkEditor)