// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.h"
#include "InteractableComponent.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UInteractableComponent : public UUserWidget
{
	GENERATED_BODY()
public:

	
	UPROPERTY(EditAnywhere)
	AActor* InteractableObject;

};
