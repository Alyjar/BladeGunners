// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	/// Returns all of the Player Names in the Server.
	UFUNCTION(BlueprintCallable, Category = "Casting")
	TArray<FString> GetPlayerNames();
};
