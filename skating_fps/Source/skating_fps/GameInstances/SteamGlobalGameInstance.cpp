// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamGlobalGameInstance.h"
#include "Engine/NetDriver.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USteamGlobalGameInstance::Init()
{
	SetMasterVolume(MasterSoundValue);
	SetMusicVolume(MusicSoundValue);
	SetVFXVolume(VFXSoundValue);
}

void USteamGlobalGameInstance::Disconnect()
{
	IOnlineSubsystem* sub = IOnlineSubsystem::Get();
	if (sub)
	{
		IOnlineSessionPtr sessions = sub->GetSessionInterface();
		if (sessions.IsValid())
			sessions.Get()->DestroySession(GameSessionName);
	}
}

void USteamGlobalGameInstance::LoadMap(TAssetPtr<UWorld> level)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(level.GetAssetName()));
}

void USteamGlobalGameInstance::LoadMapWithFailMessage(TAssetPtr<UWorld> level, FString message)
{
	LastErrorMessage = message;
	UGameplayStatics::OpenLevel(GetWorld(), FName(level.GetAssetName()));
}

void USteamGlobalGameInstance::SetLastError(FString errorMsg)
{
	LastErrorMessage = errorMsg;
}

FString USteamGlobalGameInstance::GetLastError()
{
	FString _errorMessage = LastErrorMessage;
	LastErrorMessage = "";

	return _errorMessage;
}

void USteamGlobalGameInstance::SetLastIP(FString address)
{
	LastIP = address;
}

FString USteamGlobalGameInstance::GetLastIP()
{
	return LastIP;
}

void USteamGlobalGameInstance::SetMasterVolume_Implementation(float value)
{
	MasterSoundValue = value;

	SaveConfig();
}

float USteamGlobalGameInstance::GetMasterVolume()
{
	return MasterSoundValue;
}

void USteamGlobalGameInstance::SetMusicVolume_Implementation(float value)
{
	MusicSoundValue = value;

	SaveConfig();
}

float USteamGlobalGameInstance::GetMusicVolume()
{
	return MusicSoundValue;
}

void USteamGlobalGameInstance::SetVFXVolume_Implementation(float value)
{
	VFXSoundValue = value;

	SaveConfig();
}

float USteamGlobalGameInstance::GetVFXVolume()
{
	return VFXSoundValue;
}
