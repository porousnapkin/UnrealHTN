// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNComponent.h"

#include "TestHTNComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTHTNPROJECT_API UTestHTNComponent : public UHTNComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTestHTNComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector FridgeLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector TableLocation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* UseFridgeAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* UseTableAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector DoorLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* UseDoorAnimation;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
