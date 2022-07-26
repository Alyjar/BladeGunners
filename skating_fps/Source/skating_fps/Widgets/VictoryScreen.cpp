#include "VictoryScreen.h"

#include "../PlayerStates/TeamPlayerState.h"

#include "GameFramework/GameStateBase.h"

void UVictoryScreen::SetWinningTeam(const EPlayerTeam& team)
{
	WinningTeam = team;

	SetMVPs();

	HasWon() ? OnWin() : OnLose();

	switch (team)
	{
	case EPlayerTeam::Cyan:
		OnCyanWin();
		break;

	case EPlayerTeam::Pink:
		OnPinkWin();
		break;

	case EPlayerTeam::None:
		OnTie();
		break;

	default:
		break;
	}
}

void UVictoryScreen::OnTimerRefresh_Implementation()
{
}

void UVictoryScreen::OnTie_Implementation()
{
}

void UVictoryScreen::OnCyanWin_Implementation()
{
}

void UVictoryScreen::OnPinkWin_Implementation()
{
}

void UVictoryScreen::OnWin_Implementation()
{
}

void UVictoryScreen::OnLose_Implementation()
{
}

bool UVictoryScreen::HasWon()
{
	ATeamPlayerState* tps = GetOwningPlayerState<ATeamPlayerState>();
	if (tps && tps->Team.Team == WinningTeam)
		return true;

	return false;
}

void UVictoryScreen::SetMVPs()
{
	ATeamPlayerState* cyanMVP = GetMVP(EPlayerTeam::Cyan);
	ATeamPlayerState* pinkMVP = GetMVP(EPlayerTeam::Pink);

	cyanMVP != nullptr ? SetCyanMVP(cyanMVP->UserName, cyanMVP->GetKills()) : SetCyanMVP(NullName, 0);
	pinkMVP != nullptr ? SetPinkMVP(pinkMVP->UserName, pinkMVP->GetKills()) : SetPinkMVP(NullName, 0);
}

ATeamPlayerState* UVictoryScreen::GetMVP(EPlayerTeam team)
{
	int mostKills = 0;
	ATeamPlayerState* mvp = nullptr;

	if (GetWorld() && GetWorld()->GetGameState())
	{
		TArray<APlayerState*> players = GetWorld()->GetGameState()->PlayerArray;
		for (APlayerState* player : players)
		{
			ATeamPlayerState* tps = Cast<ATeamPlayerState>(player);
			int kills = tps->GetKills();

			if (tps && tps->Team.Team == team && mostKills <= kills)
			{
				mostKills = kills;
				mvp = tps;
			}
		}
	}

	return mvp;
}

TArray<ATeamPlayerState*> UVictoryScreen::GetPlayersOnTeam(EPlayerTeam team)
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		TArray<APlayerState*> players = GetWorld()->GetGameState()->PlayerArray;
		TArray<ATeamPlayerState*> teamPlayers = TArray<ATeamPlayerState*>();
		for (APlayerState* player : players)
		{
			ATeamPlayerState* teamPlayer = Cast<ATeamPlayerState>(player);
			if (teamPlayer && teamPlayer->Team.Team == team)
				teamPlayers.Add(teamPlayer);
		}

		return teamPlayers;
	}

	return {};
}

void UVictoryScreen::SetCyanMVP_Implementation(const FString& name, int Kills)
{
}

void UVictoryScreen::SetPinkMVP_Implementation(const FString& name, int Kills)
{
}
