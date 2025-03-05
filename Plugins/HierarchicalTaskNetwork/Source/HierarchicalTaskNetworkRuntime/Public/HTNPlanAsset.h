#pragma once

#include "CoreMinimal.h"
#include "HTNPlannerBase.h"
#include "HTNPlanAsset.generated.h"

/**
 * Asset representing a saved HTN plan template.
 * This allows plans to be saved to disk and reused.
 */
UCLASS(BlueprintType)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPlanAsset : public UObject
{
    GENERATED_BODY()

public:
    UHTNPlanAsset();
    
    /** The plan data stored in this asset */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTN|Plan")
    FHTNPlan Plan;
    
    /** Optional description of this plan template */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Plan")
    FString Description;
    
    /** Tags for organizing and searching plan assets */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Plan")
    TArray<FName> Tags;
    
    /** Time when this plan template was created */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HTN|Plan")
    FDateTime CreationTime;
    
    /** Time when this plan template was last modified */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HTN|Plan")
    FDateTime LastModifiedTime;
    
    /**
     * Creates a new plan asset from a plan object.
     * 
     * @param InPlan - The plan to store
     * @param InDescription - Optional description
     * @param InTags - Optional tags
     * @param OuterObject - Outer object for the new asset (defaults to transient package)
     * @return The created plan asset
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Plan|Asset")
    static UHTNPlanAsset* CreateFromPlan(
        const FHTNPlan& InPlan, 
        const FString& InDescription,
        const TArray<FName>& InTags,
        UObject* OuterObject = nullptr);
    
    /**
     * Loads a plan asset from a JSON string.
     * 
     * @param JsonString - The JSON representation of the plan
     * @param OuterObject - Outer object for the new asset (defaults to transient package)
     * @return The created plan asset, or nullptr if loading failed
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Plan|Asset")
    static UHTNPlanAsset* LoadFromJson(const FString& JsonString, UObject* OuterObject = nullptr);
    
    /**
     * Saves the plan asset to a JSON string.
     * 
     * @return JSON string representation of the plan asset
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Plan|Asset")
    FString SaveToJson() const;
    
    /**
     * Updates the last modified time of this asset.
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Plan|Asset")
    void UpdateModifiedTime();
    
    //~ Begin UObject Interface
    virtual void Serialize(FArchive& Ar) override;
    //~ End UObject Interface
};