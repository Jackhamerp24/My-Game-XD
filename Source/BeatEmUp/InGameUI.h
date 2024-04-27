// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "InGameUI.generated.h"

/**
 * 
 */
class ABeatEmUpCharacter;

UCLASS()
class BEATEMUP_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealthText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PunchCooldown;

	UPROPERTY()
	ABeatEmUpCharacter* Player;

	void UpdateValues();
};
