// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "Blueprint/UserWidget.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	UUserWidget*MainMenu = Cast<UUserWidget>(CreateWidget(GetGameInstance(), MainMenuClass));
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	MainMenu->AddToViewport();
}
