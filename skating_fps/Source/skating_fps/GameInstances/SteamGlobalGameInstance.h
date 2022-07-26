// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "SteamGlobalGameInstance.generated.h"

class UWorld;
/**
 * 
 */
UCLASS(config = Game, transient, BlueprintType, Blueprintable)
class SKATING_FPS_API USteamGlobalGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
		void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Menus")
		void LoadMap(TAssetPtr<UWorld> level);
	UFUNCTION(BlueprintCallable, Category = "Menus")
		void LoadMapWithFailMessage(TAssetPtr<UWorld> level, FString message);

	UFUNCTION(BlueprintCallable, Category = "Menus")
		void SetLastError(FString errorMsg);
	UFUNCTION(BlueprintCallable, Category = "Menus")
		FString GetLastError();

	UFUNCTION(BlueprintCallable)
		void SetLastIP(FString address);
	UFUNCTION(BlueprintCallable)
		FString GetLastIP();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SetMasterVolume(float value);
	UFUNCTION(BlueprintCallable)
		float GetMasterVolume();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SetMusicVolume(float value);
	UFUNCTION(BlueprintCallable)
		float GetMusicVolume();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SetVFXVolume(float value);
	UFUNCTION(BlueprintCallable)
		float GetVFXVolume();

	UPROPERTY(Config, VisibleAnywhere, BlueprintReadOnly)
		FString SavedUserName = "";

private:
	UPROPERTY(EditAnywhere, Category = "Menus", meta = (AllowPrivateAccess = "true"))
		TAssetPtr<UWorld> LevelToLoad = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Menus", meta = (AllowPrivateAccess = "true"))
		FString LastErrorMessage = "";

	UPROPERTY(VisibleAnywhere)
		FString LastIP = "";

	UPROPERTY(Config, VisibleAnywhere, meta = (AllowPrivateAcess = true))
		float MasterSoundValue = 1.0f;

	UPROPERTY(Config, VisibleAnywhere, meta = (AllowPrivateAcess = true))
		float MusicSoundValue = 1.0f;

	UPROPERTY(Config, VisibleAnywhere, meta = (AllowPrivateAcess = true))
		float VFXSoundValue = 1.0f;
};
