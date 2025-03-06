// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "AssetTypeCategories.h"

class FHierarchicalTaskNetworkEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Gets the HTN asset category for the editor */
	EAssetTypeCategories::Type GetAssetCategory() const { return HTNAssetCategory; }

	/** Gets the module instance */
	static FHierarchicalTaskNetworkEditorModule& Get()
	{
		return FModuleManager::GetModuleChecked<FHierarchicalTaskNetworkEditorModule>("HierarchicalTaskNetworkEditor");
	}

private:
	/** Register the asset types that this editor module creates */
	void RegisterAssetTypes();

	/** Unregister the asset types that this editor module creates */
	void UnregisterAssetTypes();

	/** Register the asset type actions for this module */
	void RegisterAssetTypeActions(TSharedRef<class IAssetTypeActions> Action);

	/** The HTN asset category in the content browser */
	EAssetTypeCategories::Type HTNAssetCategory;

	/** All created asset type actions. Cached for cleanup. */
	TArray< TSharedPtr<class IAssetTypeActions> > CreatedAssetTypeActions;
};
