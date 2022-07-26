#include "ScoreboardPlayerInfo.h"

#include "../PlayerStates/MainPlayerState.h"
#include "../Movement/PlayerMovement.h"

void UScoreboardPlayerInfo::SetPlayerByState(AMainPlayerState* playerstate)
{
	if (playerstate)
	{
		SetPlayerName(playerstate->UserName);
		SetIsAlive(playerstate->IsAlive);
		SetPlayerKills(playerstate->GetKills());
		SetPlayerDeaths(playerstate->GetDeaths());
		SetPlayerPing(playerstate->GetPing());
	}
}

void UScoreboardPlayerInfo::SetPlayerName_Implementation(const FString& name)
{
}

void UScoreboardPlayerInfo::SetIsAlive_Implementation(bool value)
{
}

void UScoreboardPlayerInfo::SetPlayerKills_Implementation(int kills)
{
}

void UScoreboardPlayerInfo::SetPlayerDeaths_Implementation(int deaths)
{
}

void UScoreboardPlayerInfo::SetPlayerPing_Implementation(int ms)
{
}