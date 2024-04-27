// Fill out your copyright notice in the Description page of Project Settings.
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnFrequency, true);

}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::SpawnEnemy()
{
	TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Enemies);
    if(EnemyClass && Enemies.Num() < MaxEnemies)
    {
     FVector SpawnLocation = FVector(GetActorLocation().X + FMath::RandRange(-SpawnRadius, SpawnRadius), GetActorLocation().Y + 
    FMath::RandRange(-SpawnRadius, SpawnRadius), GetActorLocation().Z);
     FRotator SpawnRotation = GetActorRotation();
     FActorSpawnParameters SpawnParams;
     SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
     GetWorld()->SpawnActor(EnemyClass, &SpawnLocation, &SpawnRotation, SpawnParams);
    }

}

