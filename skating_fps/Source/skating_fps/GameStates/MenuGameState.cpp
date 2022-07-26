// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGameState.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

#include "../GameInstances/GlobalGameInstance.h"
#include "../Widgets/InfoBox.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../GameInstances/SteamGlobalGameInstance.h"

void AMenuGameState::LoadLastErrorMessage()
{
	UGameInstance* globalInstance = UGameplayStatics::GetGameInstance(GetWorld());
	USteamGlobalGameInstance* gInstance = Cast<USteamGlobalGameInstance>(globalInstance);

	FString message = gInstance->GetLastError();
	if (message.Len() > 0)
	{
		UInfoBox* infoBox = CreateWidget<UInfoBox>(GetWorld(), InfoBoxBP, TEXT("Info Box"));
		infoBox->AddToViewport();
		infoBox->SetMessage(message);
	}
}
