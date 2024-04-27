// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatEmUpCharacter.h"
#include "DamageTextUI.h"
#include "Enemy.generated.h"

UCLASS()
class BEATEMUP_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY()
     ABeatEmUpCharacter* Player;
    UPROPERTY(EditAnywhere)
     float MaxHealth = 100;
    UPROPERTY(EditAnywhere)
     float CurrentHealth = MaxHealth;
	 void DealDamage(float Damage);
	 UFUNCTION()
	 void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal, const FHitResult& Hit);

	 UPROPERTY(EditAnywhere)
	 float DamageToPlayer = 10;
	 UPROPERTY(EditAnywhere)
	 float RagdollTime = 5;
	 FTimerHandle RagdollTimerHandle;
	 void Ragdoll();
	 UFUNCTION()
	 void StopRagdoll();

	 UPROPERTY(EditAnywhere)
 		TSubclassOf<UDamageTextUI> DamageTextClass;
};
