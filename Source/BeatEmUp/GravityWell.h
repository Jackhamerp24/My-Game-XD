// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Interactable.h"
#include "Enemy.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravityWell.generated.h"

UENUM()
enum class EGravityType:uint8
{
	GE_Pull UMETA(DisplayName = "Pull"),
	GE_Push UMETA(DisplayName = "Push"),
	GE_On UMETA(DisplayName = "On"),
	GE_Off UMETA(DisplayName = "Off")
};
UCLASS()
class BEATEMUP_API AGravityWell : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityWell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
	EGravityType GravityType;
	UPROPERTY(EditAnywhere)
	float SweepSize = 1000;
	UPROPERTY(EditAnywhere)
	float Force = 2000;

	void Activate();
	virtual void Interact_Implementation() override;

};
