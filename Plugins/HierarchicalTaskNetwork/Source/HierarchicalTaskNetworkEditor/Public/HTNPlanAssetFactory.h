#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "HTNPlanAssetFactory.generated.h"

/**
 * Factory for creating HTN Plan Assets in the editor.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNPlanAssetFactory : public UFactory
{
	GENERATED_BODY()
    
public:
	UHTNPlanAssetFactory();
    
	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
	//~ End UFactory Interface
};