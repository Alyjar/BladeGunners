#include "MainHUD.h"

#include "../GameStates/MainLevelGameState.h"
#include "../PlayerStates/MainPlayerState.h"
#include "../Widgets/KillFeedBox.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/VerticalBox.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	AMainLevelGameState* gameState = GetMainGameState();

	if (gameState)
	{
		gameState->OnPlayerKill.AddDynamic(this, &UMainHUD::RefreshKillCount);
		gameState->OnMatchTimerChange.AddDynamic(this, &UMainHUD::RefreshTimerText);
		gameState->OnPlayerNameKill.AddDynamic(this, &UMainHUD::AddKillToFeed);
	}

	RefreshKillCount();
	RefreshTimerText();
}


void UMainHUD::RefreshKillCount()
{
	AMainLevelGameState* gameState = GetMainGameState();

	if (gameState)
	{
		int cyanKC = gameState->GetTeamKillCount(EPlayerTeam::Cyan);
		int pinkKC = gameState->GetTeamKillCount(EPlayerTeam::Pink);

		if (CyanKillCountText)
			CyanKillCountText->SetText(FText::FromString(FString::FromInt(cyanKC)));

		if (PinkKillCountText)
			PinkKillCountText->SetText(FText::FromString(FString::FromInt(pinkKC)));
	}
}

void UMainHUD::RefreshTimerText()
{
	AMainLevelGameState* gameState = GetMainGameState();
	if (TimerText && gameState)
	{
		int timeInSeconds = FMath::RoundToInt(gameState->CurrentGameTimer);

		int minutes = timeInSeconds / 60;
		int seconds = timeInSeconds % 60;

		FString secondsStr = (seconds < 10) ? "0" : "";
		secondsStr += FString::FromInt(seconds);

		FString str = FString::FromInt(minutes) + ":" + secondsStr;

		TimerText->SetText(FText::FromString(str));
	}

	if (!TimerText)
		UKismetSystemLibrary::PrintString(this, "NULL");
}

void UMainHUD::AddKillToFeed(const AMainPlayerState* initiator, const AMainPlayerState* victim, const EWeapon& weapon, bool isHeadshot)
{
	if (!KillFeedTemplate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "KILL FEED TEMPLATE NOT ASSIGNED!");
		return;
	}

	if (!KillFeedPanel)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "KILL FEED PANEL INSTANCE NOT ASSIGNED!");
		return;
	}

	UKillFeedBox* killBox = CreateWidget<UKillFeedBox>(KillFeedPanel, KillFeedTemplate);

	TArray<UWidget*> children = KillFeedPanel->GetAllChildren();
	children.Insert(killBox, 0);

	KillFeedPanel->ClearChildren();

	for (UWidget* child : children)
		KillFeedPanel->AddChild(child);

	killBox->SetInitiator(initiator);
	killBox->SetVictim(victim);

	killBox->SetWeaponIcon(weapon);
	killBox->SetHeadshotIcon(isHeadshot);
}

AMainLevelGameState* UMainHUD::GetMainGameState()
{
	return Cast<AMainLevelGameState>(UGameplayStatics::GetGameState(this));
}