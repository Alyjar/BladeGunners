#include "MainPlayerState.h"

#include "../GameInstances/GlobalGameInstance.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "../GameInstances/SteamGlobalGameInstance.h"

AMainPlayerState::AMainPlayerState()
{
	SetReplicates(true);
}

void AMainPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayerState, UserName);
	DOREPLIFETIME(AMainPlayerState, IsAlive);
	DOREPLIFETIME(AMainPlayerState, Kills);
	DOREPLIFETIME(AMainPlayerState, Deaths);
}

void AMainPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialiseUserName();
}

void AMainPlayerState::InitialiseUserName()
{
	// Checking to see if it is Client and if that Client is the Playerstate.
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (controller && IsOwnedBy(controller))
	{
		Server_UpdateUserName(GetSavedUserName());
		OnInitialised.Broadcast();
	}
}

void AMainPlayerState::Server_UpdateUserName_Implementation(const FString& name)
{
	this->UserName = name;
	OnNameInitialised.Broadcast(name);
}

void AMainPlayerState::SetSavedUserName(FString name)
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (controller && !IsOwnedBy(controller))
		return;

	USteamGlobalGameInstance* instance = GetWorld()->GetGameInstance<USteamGlobalGameInstance>();
	if (instance)
	{
		instance->SavedUserName = name;
		instance->SaveConfig();
	}

	Server_UpdateUserName(name);
}

FString AMainPlayerState::GetSavedUserName()
{
	USteamGlobalGameInstance* instance = GetWorld()->GetGameInstance<USteamGlobalGameInstance>();
	if (instance)
		return instance->SavedUserName;
		
	return "NULL";
}

void AMainPlayerState::AddKill()
{
	Kills++;
}

int AMainPlayerState::GetKills()
{
	return Kills;
}

void AMainPlayerState::AddDeath()
{
	Deaths++;
}

int AMainPlayerState::GetDeaths()
{
	return Deaths;
}
