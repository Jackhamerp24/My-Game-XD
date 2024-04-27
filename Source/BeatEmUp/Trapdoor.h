// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Interactable.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trapdoor.generated.h"

UCLASS()
class BEATEMUP_API ATrapdoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapdoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LeftDoor;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* RightDoor;
	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* LeftHinge;
	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* RightHinge;
	virtual void Interact_Implementation() override;
};
