// Fill out your copyright notice in the Description page of Project Settings.


#include "Trapdoor.h"

// Sets default values
ATrapdoor::ATrapdoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Door"));
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Door"));
	LeftHinge = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Left Hinge"));
	RightHinge = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Right Hinge"));
	LeftDoor->SetupAttachment(RootComponent);
	RightDoor->SetupAttachment(RootComponent);
	LeftHinge->SetupAttachment(RootComponent);
	RightHinge->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATrapdoor::BeginPlay()
{
	Super::BeginPlay();
	LeftDoor->SetSimulatePhysics(false);
	RightDoor->SetSimulatePhysics(false);
}

// Called every frame
void ATrapdoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrapdoor::Interact_Implementation()
{
	IInteractable::Interact_Implementation();
	LeftDoor->SetSimulatePhysics(true);
	RightDoor->SetSimulatePhysics(true);
	LeftDoor->AddImpulse
	(FVector::DownVector);
	RightDoor->AddImpulse
	(FVector::DownVector);
}

