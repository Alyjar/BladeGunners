// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameState.h"

#include "GameFramework/PlayerState.h"


/******************************************************************************
* Casts the PlayerArray to an Array of FString with the Names of the Players/
******************************************************************************/
TArray<FString> ALobbyGameState::GetPlayerNames()
{
	TArray<FString> playerStrs = {};
	for (APlayerState* player : PlayerArray)
	{
		if (player)
			playerStrs.Add(player->GetPlayerNameCustom());
	}

	return playerStrs;
}
