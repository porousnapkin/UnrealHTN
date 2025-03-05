#include "HTNPlanAssetFactory.h"
#include "HTNPlanAsset.h"

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
		NewAsset->UpdateModifiedTime();
	}
    
	return NewAsset;
}

bool UHTNPlanAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}