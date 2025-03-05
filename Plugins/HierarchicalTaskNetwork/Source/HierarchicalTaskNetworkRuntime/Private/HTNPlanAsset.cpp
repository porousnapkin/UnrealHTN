// HTNPlanAsset.cpp

#include "HTNPlanAsset.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Misc/FileHelper.h"
#include "HTNLogging.h"

UHTNPlanAsset::UHTNPlanAsset()
{
    CreationTime = FDateTime::Now();
    LastModifiedTime = CreationTime;
}

UHTNPlanAsset* UHTNPlanAsset::CreateFromPlan(
    const FHTNPlan& InPlan, 
    const FString& InDescription,
    const TArray<FName>& InTags,
    UObject* OuterObject)
{
    if (!OuterObject)
    {
        OuterObject = GetTransientPackage();
    }
    
    UHTNPlanAsset* PlanAsset = NewObject<UHTNPlanAsset>(OuterObject);
    if (PlanAsset)
    {
        PlanAsset->Plan = InPlan;
        PlanAsset->Description = InDescription;
        PlanAsset->Tags = InTags;
        PlanAsset->CreationTime = FDateTime::Now();
        PlanAsset->LastModifiedTime = PlanAsset->CreationTime;
    }
    
    return PlanAsset;
}

UHTNPlanAsset* UHTNPlanAsset::LoadFromJson(const FString& JsonString, UObject* OuterObject)
{
    if (!OuterObject)
    {
        OuterObject = GetTransientPackage();
    }
    
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("Failed to parse plan asset JSON"));
        return nullptr;
    }
    
    UHTNPlanAsset* PlanAsset = NewObject<UHTNPlanAsset>(OuterObject);
    if (!PlanAsset)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("Failed to create plan asset object"));
        return nullptr;
    }
    
    // Read description
    if (JsonObject->HasField(TEXT("Description")))
    {
        PlanAsset->Description = JsonObject->GetStringField(TEXT("Description"));
    }
    
    // Read tags
    if (JsonObject->HasField(TEXT("Tags")))
    {
        const TArray<TSharedPtr<FJsonValue>>* TagsArray;
        if (JsonObject->TryGetArrayField(TEXT("Tags"), TagsArray))
        {
            for (const TSharedPtr<FJsonValue>& Value : *TagsArray)
            {
                PlanAsset->Tags.Add(FName(*Value->AsString()));
            }
        }
    }
    
    // Read creation time
    if (JsonObject->HasField(TEXT("CreationTime")))
    {
        FString CreationTimeStr = JsonObject->GetStringField(TEXT("CreationTime"));
        FDateTime::Parse(CreationTimeStr, PlanAsset->CreationTime);
    }
    else
    {
        PlanAsset->CreationTime = FDateTime::Now();
    }
    
    // Read last modified time
    if (JsonObject->HasField(TEXT("LastModifiedTime")))
    {
        FString LastModifiedTimeStr = JsonObject->GetStringField(TEXT("LastModifiedTime"));
        FDateTime::Parse(LastModifiedTimeStr, PlanAsset->LastModifiedTime);
    }
    else
    {
        PlanAsset->LastModifiedTime = PlanAsset->CreationTime;
    }
    
    // Read plan data
    if (JsonObject->HasField(TEXT("Plan")))
    {
        const TSharedPtr<FJsonObject>* PlanObject;
        if (JsonObject->TryGetObjectField(TEXT("Plan"), PlanObject))
        {
            // Create a new string to hold the serialized JSON
            FString PlanJsonString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PlanJsonString);
            FJsonSerializer::Serialize(PlanObject->ToSharedRef(), Writer);
            
            if (!PlanAsset->Plan.FromJson(JsonString))
            {
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Failed to parse plan data from JSON"));
            }
        }
    }
    
    return PlanAsset;
}

FString UHTNPlanAsset::SaveToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    
    // Add description
    JsonObject->SetStringField(TEXT("Description"), Description);
    
    // Add tags
    TArray<TSharedPtr<FJsonValue>> TagsArray;
    for (const FName& Tag : Tags)
    {
        TagsArray.Add(MakeShared<FJsonValueString>(Tag.ToString()));
    }
    JsonObject->SetArrayField(TEXT("Tags"), TagsArray);
    
    // Add creation time
    JsonObject->SetStringField(TEXT("CreationTime"), CreationTime.ToString());
    
    // Add last modified time
    JsonObject->SetStringField(TEXT("LastModifiedTime"), LastModifiedTime.ToString());
    
    // Add plan data
    FString PlanJson = Plan.ToJson();
    TSharedPtr<FJsonObject> PlanJsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PlanJson);
    if (FJsonSerializer::Deserialize(Reader, PlanJsonObject) && PlanJsonObject.IsValid())
    {
        JsonObject->SetObjectField(TEXT("Plan"), PlanJsonObject);
    }
    else
    {
        // Fallback to string if parsing fails
        JsonObject->SetStringField(TEXT("PlanRaw"), PlanJson);
    }
    
    // Convert to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    return OutputString;
}

void UHTNPlanAsset::UpdateModifiedTime()
{
    LastModifiedTime = FDateTime::Now();
}

void UHTNPlanAsset::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    
    // Version for serialization format
    int32 Version = 1;
    Ar << Version;
    
    // Serialize plan data
    if (Ar.IsLoading())
    {
        // When loading, we first serialize to a binary buffer
        TArray<uint8> PlanData;
        Ar << PlanData;
        
        if (PlanData.Num() > 0)
        {
            Plan.FromBinary(PlanData);
        }
    }
    else if (Ar.IsSaving())
    {
        // When saving, we convert the plan to binary
        TArray<uint8> PlanData;
        Plan.ToBinary(PlanData);
        Ar << PlanData;
    }
    
    // Serialize metadata
    Ar << Description;
    Ar << Tags;
    Ar << CreationTime;
    Ar << LastModifiedTime;
}