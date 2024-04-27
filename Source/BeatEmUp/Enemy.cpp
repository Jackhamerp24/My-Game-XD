// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Player)
	{
		FVector TowardsPlayer = Player->GetActorLocation() - GetActorLocation();
		AddMovementInput(TowardsPlayer);
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::DealDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	UDamageTextUI* SpawnedDamage = Cast<UDamageTextUI>(CreateWidget(GetGameInstance(),DamageTextClass));
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), GetMesh()->GetComponentLocation(),SpawnedDamage->CurrentLocation);
	SpawnedDamage->DamageText->SetText(FText::FromString(FString::SanitizeFloat(Damage)));
	SpawnedDamage->TargetLocation += SpawnedDamage->CurrentLocation;
	SpawnedDamage->AddToViewport();
}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit)
{
	if (OtherActor == Player
		)
	{
		Player->DealDamage(DamageToPlayer);
		Destroy();
	}

}

void AEnemy::Ragdoll()
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName("NoCollision");
	GetWorld()->GetTimerManager().SetTimer(RagdollTimerHandle, this, &AEnemy::StopRagdoll, RagdollTime, false);
}

void AEnemy::StopRagdoll()
{
	if (CurrentHealth <= 0)
	{
		Destroy();
		return;
	}
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetSocketLocation("pelvis"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");

}

