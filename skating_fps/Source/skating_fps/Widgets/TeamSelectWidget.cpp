#include "TeamSelectWidget.h"

#include "../PlayerStates/TeamPlayerState.h"
#include "../GameStates/MainLevelGameState.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

void UTeamSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LevelGameState = GetWorld()->GetGameState<AMainLevelGameState>();
}

void UTeamSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	SetNamesForTeam(EPlayerTeam::Cyan);
	SetNamesForTeam(EPlayerTeam::Pink);

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UTeamSelectWidget::AssignUserNameTextBlock(EPlayerTeam team, UPanelWidget* parent)
{
	TArray<UTextBlock*> names = TArray<UTextBlock*>();
	for (UWidget* widget : parent->GetAllChildren())
	{
		if (UTextBlock* text = Cast<UTextBlock>(widget))
			names.Add(text);
	}

	if (team == EPlayerTeam::Pink)
		PinkTeamNames = names;

	else if (team == EPlayerTeam::Cyan)
		CyanTeamNames = names;
}

void UTeamSelectWidget::RequestSpawn_Implementation(EPlayerTeam team)
{
	int currentPlayers = GetTeamNumber(team);
	int maxPlayers = GetTeamInfo(team).MaxPlayers;

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (currentPlayers < maxPlayers)
	{
		OnTeamSelect.Broadcast(team);
	}
}

void UTeamSelectWidget::RandomRequestSpawn()
{
	// If I needed to scale this to multiple teams I would have this method
	// call itself on the 'else' statements and pass in a flag/int to determine
	// what as previously been called. Or maybe I'd use a a regular while loop.

	int cyanPlayers = GetTeamNumber(EPlayerTeam::Cyan);
	int maxCyan = GetTeamInfo(EPlayerTeam::Cyan).MaxPlayers;

	int pinkPlayers = GetTeamNumber(EPlayerTeam::Pink);
	int maxPink = GetTeamInfo(EPlayerTeam::Pink).MaxPlayers;

	int random = FMath::RandRange(0, 1);

	// If Cyan is selected and isn't capped.
	if (random == 0 && cyanPlayers < maxCyan)
	{
		RequestSpawn(EPlayerTeam::Cyan);
		return;
	}

	// Cyan selected and is capped.
	// No need to check since it will fail later if it Pink is capped.
	// This means that both teams are full and nothing happens.
	else
	{
		RequestSpawn(EPlayerTeam::Pink);
		return;
	}

	// If Pink is selected and isn't capped
	if (random == 1 && pinkPlayers < maxPink)
	{
		RequestSpawn(EPlayerTeam::Pink);
		return;
	}

	// Pink selected and capped.
	// Same prior logic still applies.
	else
	{
		RequestSpawn(EPlayerTeam::Cyan);
		return;
	}
}

void UTeamSelectWidget::RequestSpectator()
{
	OnSpectatorSelect.Broadcast();
}

int UTeamSelectWidget::GetTeamNumber(EPlayerTeam team)
{
	AGameState* gameState = GetWorld()->GetGameState<AGameState>();

	int count = 0;
	for (APlayerState* state : gameState->PlayerArray)
	{
		ATeamPlayerState* playerTeamState = Cast<ATeamPlayerState>(state);
		if (playerTeamState && playerTeamState->Team.Team == team)
			count++;
	}

	return count;
}

void UTeamSelectWidget::SetTextForTeam(UTextBlock* text, EPlayerTeam team)
{
	if (!text)
		return;

	int currentPlayers = GetTeamNumber(team);

	FTeamData data = GetTeamInfo(team);
	FString txt = FString(FString::FromInt(currentPlayers) + " / " + FString::FromInt(data.MaxPlayers));
	text->SetText(FText::FromString(txt));
}

void UTeamSelectWidget::SetBottomText(UTextBlock* text)
{
	if (text)
	{
		FString txt = FString();

		if (LevelGameState && LevelGameState->IsGameReady())
		{
			int selectedPlayers = GetTeamNumber(EPlayerTeam::Cyan) + GetTeamNumber(EPlayerTeam::Pink);
			int maxPlayers = GetTeamInfo(EPlayerTeam::Cyan).MaxPlayers + GetTeamInfo(EPlayerTeam::Pink).MaxPlayers;

			txt = FString(FString::FromInt(selectedPlayers) + " / " + FString::FromInt(maxPlayers));
		}

		else if (LevelGameState)
			txt = FString::FromInt(FMath::RoundToInt(LevelGameState->CurrentSelectTimer));
		
		text->SetText(FText::FromString(txt));
	}
}

void UTeamSelectWidget::SetNamesForTeam(EPlayerTeam team)
{
	if (team != EPlayerTeam::Cyan && team != EPlayerTeam::Pink)
		return;

	TArray<ATeamPlayerState*> players = GetPlayersOnTeam(team);
	TArray<UTextBlock*>& names = (team == EPlayerTeam::Cyan) ? CyanTeamNames : PinkTeamNames;
	for (int i = 0; i < names.Num(); i++)
	{
		names[i]->SetText(FText::FromString(EmptyName));

		if (players.Num() > 0)
		{
			ATeamPlayerState* tps = Cast<ATeamPlayerState>(players[i]);
			if (tps && tps->Team.Team == team)
				names[i]->SetText(FText::FromString(tps->UserName));

			players.Pop();
		}
	}
}

void UTeamSelectWidget::SetRandomButtonState(UButton* button)
{
	if (OwningTeamState)
	{
		bool isCyanFull = IsTeamFull(EPlayerTeam::Cyan);
		bool isPinkFull = IsTeamFull(EPlayerTeam::Pink);

		bool isOnTeam = OwningTeamState->Team.Team == EPlayerTeam::Cyan || OwningTeamState->Team.Team == EPlayerTeam::Pink;

		if (button)
			button->SetIsEnabled(!(isCyanFull && isPinkFull) && !isOnTeam);
	}

	else
		OwningTeamState = GetOwningPlayer()->GetPlayerState<ATeamPlayerState>();
}

void UTeamSelectWidget::SetTeamButtonState(UButton* button, EPlayerTeam team)
{
	if (OwningTeamState)
	{
		bool full = IsTeamFull(team);
		bool isOnTeam = (OwningTeamState->Team.Team == team);

		if (button)
			button->SetIsEnabled(!full && !isOnTeam);
	}

	else
		OwningTeamState = GetOwningPlayer()->GetPlayerState<ATeamPlayerState>();
}

void UTeamSelectWidget::SetSpectatorButtonState(UButton* button)
{
	if (OwningTeamState)
	{

		if (button && LevelGameState)
			button->SetIsEnabled(!(OwningTeamState->Team.Team == EPlayerTeam::Spectator) && LevelGameState->IsGameReady());
	}

	else
		OwningTeamState = GetOwningPlayer()->GetPlayerState<ATeamPlayerState>();
}

FTeamData UTeamSelectWidget::GetTeamInfo(EPlayerTeam team)
{
	for (FTeamData data : Teams)
		if (data.Team == team)
			return data;

	return FTeamData();
}

void UTeamSelectWidget::CloseWindow()
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	controller->SetInputMode(FInputModeGameOnly());
	controller->bShowMouseCursor = false;

	RemoveFromParent();
}

bool UTeamSelectWidget::IsTeamFull(EPlayerTeam team)
{
	int currentPlayers = GetTeamNumber(team);
	int maxPlayers = GetTeamInfo(team).MaxPlayers;

	return currentPlayers == maxPlayers;
}

TArray<ATeamPlayerState*> UTeamSelectWidget::GetPlayersOnTeam(EPlayerTeam team)
{
	TArray<ATeamPlayerState*> teamPlayers = TArray<ATeamPlayerState*>();

	AGameStateBase* gs = GetWorld()->GetGameState();
	const TArray<APlayerState*> players = gs->PlayerArray;

	for (APlayerState* player : players)
	{
		if (ATeamPlayerState* tps = Cast<ATeamPlayerState>(player))
			if (tps->Team.Team == team)
				teamPlayers.Add(tps);
	}

	return teamPlayers;
}
