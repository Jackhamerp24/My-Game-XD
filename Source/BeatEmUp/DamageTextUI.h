// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DamageTextUI.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UDamageTextUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
 		UTextBlock* DamageText;
	UPROPERTY(EditAnywhere)
 		FVector2D TargetLocation = FVector2D(0, -200);
	FVector2D CurrentLocation;
	float CurrentOpacity =1;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
};
