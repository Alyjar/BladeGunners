#include "Scoreboard.h"

#include "../PlayerStates/TeamPlayerState.h"

#include "GameFramework/GameStateBase.h"

void UScoreboard::NativeConstruct()
{
	GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UScoreboard::RefreshScoreboard,
										   RefereshTimerDuration, true);

	Super::NativeConstruct();
}

void UScoreboard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UScoreboard::RefreshScoreboard()
{
	TArray<ATeamPlayerState*> cyan = GetPlayersOnTeam(EPlayerTeam::Cyan);
	TArray<ATeamPlayerState*> pink = GetPlayersOnTeam(EPlayerTeam::Pink);

	// Final Answer ty: https://forums.unrealengine.com/t/how-to-sort-tarray-in-c/465585/7
	// Never mind it doesn't actually work...
	// Actually it might have, but my UE4 built binaries wrong?
	//cyan.Sort([](ATeamPlayerState& a, ATeamPlayerState& b) 
	//{
	//	return  a.GetKills() < b.GetKills();
	//});

	//pink.Sort([](ATeamPlayerState& a, ATeamPlayerState& b)
	//{
	//	return  a.GetKills() < b.GetKills();
	//});

	/*Algo::Sort(cyan, &AMainPlayerState::GetKills, TGreater<>());
	Algo::Sort(pink, &AMainPlayerState::GetKills, TGreater<>());*/

	BubbleSort(cyan);
	BubbleSort(pink);

	SetCyanTeamButtons(cyan);
	SetPinkTeamButtons(pink);
}

TArray<ATeamPlayerState*> UScoreboard::GetPlayersOnTeam(EPlayerTeam team)
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		TArray<ATeamPlayerState*> teamPlayers = TArray<ATeamPlayerState*>();
		TArray<APlayerState*> players = GetWorld()->GetGameState()->PlayerArray;

		for (APlayerState* player : players)
		{
			ATeamPlayerState* tps = Cast<ATeamPlayerState>(player);
			if (tps && tps->Team.Team == team)
				teamPlayers.Add(tps);
		}

		return teamPlayers;
	}

	return {};
}

void UScoreboard::BubbleSort(TArray<ATeamPlayerState*>& orig)
{
	// Thank you: https://www.geeksforgeeks.org/bubble-sort/
	bool swapped;
	for (int i = 0; i < orig.Num() - 1; i++)
	{
		swapped = false;
		for (int j = 0; j < orig.Num() - i - 1; j++)
		{
			if (orig[j]->GetKills() < orig[j + 1]->GetKills())
			{
				orig.Swap(j, j + 1);
				swapped = true;
			}
		}

		// IF no two elements were swapped by inner loop, then break
		if (swapped == false)
			break;
	}

}

void UScoreboard::SetCyanTeamButtons_Implementation(const TArray<ATeamPlayerState*>& players)
{
}

void UScoreboard::SetPinkTeamButtons_Implementation(const TArray<ATeamPlayerState*>& players)
{
}
