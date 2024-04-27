// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityWell.h"

// Sets default values
AGravityWell::AGravityWell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void AGravityWell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGravityWell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGravityWell::Activate()
{
	TArray<FHitResult> Hits;
	TArray<AActor*> HitActors;
	FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(SweepSize);
	bool bSweep = GetWorld()->SweepMultiByChannel(Hits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, ExplosionSphere);

	if(bSweep)
	{
		for(FHitResult Hit : Hits)
		{
			if(!HitActors.Contains(Hit.GetActor()))
			{
				HitActors.Add(Hit.GetActor());
				UMeshComponent* HitMesh;
				AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor());
				if(HitEnemy)
				{
					HitMesh = Cast<UMeshComponent>(HitEnemy->GetMesh());
					HitEnemy->Ragdoll();
				}
				else
				{
					HitMesh = Cast<UMeshComponent>(Hit.GetActor()->GetRootComponent());
				}
					if(HitMesh)
					{
						switch(GravityType)
						{
						case EGravityType::GE_Pull:
							HitMesh->AddRadialImpulse(GetActorLocation(), SweepSize, Force, RIF_Linear, true);
							break;
						case EGravityType::GE_Push:
							HitMesh->AddRadialImpulse(GetActorLocation(), SweepSize, -Force, RIF_Linear, true);
							break;
						case EGravityType::GE_On:
							if(!HitEnemy)
							{
								HitMesh->SetEnableGravity(true);
							}
							break;
						case EGravityType::GE_Off:
							if(!HitEnemy)
							{
								HitMesh->SetEnableGravity(false);
							}
							break;
						default:
							UE_LOG(LogTemp, Warning, TEXT("No Gravity Type! How did this happen!?"));
						}
					}
			}
		}
	}
}
	void AGravityWell::Interact_Implementation()
	{
		IInteractable::Interact_Implementation();
		Activate();
	}



