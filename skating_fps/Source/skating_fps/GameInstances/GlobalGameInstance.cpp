#include "GlobalGameInstance.h"

#include "Engine/NetDriver.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UGlobalGameInstance::Init() 
{
	SetMasterVolume(MasterSoundValue);
	SetMusicVolume(MusicSoundValue);
	SetVFXVolume(VFXSoundValue);
}

void UGlobalGameInstance::Disconnect()
{
	IOnlineSubsystem* sub = IOnlineSubsystem::Get();
	if (sub)
	{
		IOnlineSessionPtr sessions = sub->GetSessionInterface();
		if (sessions.IsValid())
			sessions.Get()->DestroySession(GameSessionName);
	}
}

void UGlobalGameInstance::LoadMap(TAssetPtr<UWorld> level)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(level.GetAssetName()));
}

void UGlobalGameInstance::LoadMapWithFailMessage(TAssetPtr<UWorld> level, FString message)
{
	LastErrorMessage = message;
	UGameplayStatics::OpenLevel(GetWorld(), FName(level.GetAssetName()));
}

void UGlobalGameInstance::SetLastError(FString errorMsg)
{
	LastErrorMessage = errorMsg;
}

FString UGlobalGameInstance::GetLastError()
{
	FString _errorMessage = LastErrorMessage;
	LastErrorMessage = "";

	return _errorMessage;
}

void UGlobalGameInstance::SetLastIP(FString address)
{
	LastIP = address;
}

FString UGlobalGameInstance::GetLastIP()
{
	return LastIP;
}

void UGlobalGameInstance::SetMasterVolume_Implementation(float value)
{
	MasterSoundValue = value;
	
	SaveConfig();
}

float UGlobalGameInstance::GetMasterVolume()
{
	return MasterSoundValue;
}

void UGlobalGameInstance::SetMusicVolume_Implementation(float value)
{
	MusicSoundValue = value;

	SaveConfig();
}

float UGlobalGameInstance::GetMusicVolume()
{
	return MusicSoundValue;
}

void UGlobalGameInstance::SetVFXVolume_Implementation(float value)
{
	VFXSoundValue = value;

	SaveConfig();
}

float UGlobalGameInstance::GetVFXVolume()
{
	return VFXSoundValue;
}
