// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextUI.h"

void UDamageTextUI::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
    CurrentLocation = FMath::Lerp(CurrentLocation, TargetLocation, InDeltaTime);
    CurrentOpacity = FMath::Lerp(CurrentOpacity,0, InDeltaTime);
    DamageText->SetOpacity(CurrentOpacity);
    SetPositionInViewport(CurrentLocation);
    if(FVector2D::Distance(CurrentLocation, TargetLocation) <5)
    {
        RemoveFromParent(); 
        }
}

void UDamageTextUI::NativeConstruct()
{
    DamageText->SetColorAndOpacity(FLinearColor(FMath::RandRange(0,1), FMath::RandRange(0,1), FMath::RandRange(0,1), 1));
}
