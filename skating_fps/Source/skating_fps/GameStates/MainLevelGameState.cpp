#include "MainLevelGameState.h"

#include "../PlayerStates/TeamPlayerState.h"
#include "../FunctionLibraries/General.h"
#include "../Widgets/VictoryScreen.h"

#include "Blueprint/UserWidget.h" 	
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "skating_fps/Movement/PlayerMovement.h"
#include "skating_fps/Weapons/WeaponBase.h"

AMainLevelGameState::AMainLevelGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	TeamData =
	{
		FTeamData(EPlayerTeam::Cyan),
		FTeamData(EPlayerTeam::Pink),
	};
}

void AMainLevelGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainLevelGameState, TeamData);

	DOREPLIFETIME(AMainLevelGameState, IsSelectCountingDown);
	DOREPLIFETIME(AMainLevelGameState, IsMatchCountingDown);
	DOREPLIFETIME(AMainLevelGameState, IsEndCountingDown);

	DOREPLIFETIME(AMainLevelGameState, DefaultGameTimer);
	DOREPLIFETIME(AMainLevelGameState, CurrentGameTimer);

	DOREPLIFETIME(AMainLevelGameState, DefaultSelectTimer);
	DOREPLIFETIME(AMainLevelGameState, CurrentSelectTimer);

	DOREPLIFETIME(AMainLevelGameState, DefaultEndTimer);
	DOREPLIFETIME(AMainLevelGameState, CurrentEndTimer);

	DOREPLIFETIME(AMainLevelGameState, GameSkipped);
}

void AMainLevelGameState::BeginPlay()
{
	// Thanks Epic: https://forums.unrealengine.com/t/network-client-calls-beginplay-twice-on-gamestate/312869
	if (BeginPlayCalled)
		return;

	if (HasAuthority())
		StartSelectTimer();

	// It won't run on Clients since they connect
	// after the RPC is called. This is the fix.
	if (IsSelectCountingDown && !HasAuthority())
		OnPreGameStart();

	BeginPlayCalled = true;
}

void AMainLevelGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Pre-Game
	if (IsSelectCountingDown)
		CountSelectTimerDown(DeltaTime);

	// Game Duration
	if (IsMatchCountingDown && HasAuthority())
	{
		CountMatchTimerDown(DeltaTime);

		if (IsGameOver())
		{
			WinningTeam = GetWinningTeam();
			IsMatchCountingDown = false;

			Server_UnPossessAll();
			Server_LoadEndScreen(WinningTeam);
			StartEndTimer();
		}
	}

	if (IsEndCountingDown)
		CountEndTimerDown(DeltaTime);
}

void AMainLevelGameState::StartSelectTimer()
{
	IsSelectCountingDown = true;
	CurrentSelectTimer = DefaultSelectTimer;

	Multi_OnPreGameStart();
}

void AMainLevelGameState::CountSelectTimerDown(float DeltaTime)
{
	CurrentSelectTimer -= DeltaTime;
	Multi_InvokeOnSelectTimerChange();

	if (CurrentSelectTimer <= 0)
	{
		IsSelectCountingDown = false;
		Multi_InvokeOnSelectTimerEnd();

		StartMatchTimer();
	}
}

bool AMainLevelGameState::IsGameReady()
{
	return !IsSelectCountingDown;
}

void AMainLevelGameState::StartMatchTimer()
{
	IsMatchCountingDown = true;
	CurrentGameTimer = DefaultGameTimer;

	Multi_OnMatchStart();
}

void AMainLevelGameState::CountMatchTimerDown(float DeltaTime)
{
	CurrentGameTimer -= DeltaTime;
	Multi_InvokeOnMatchTimerChange();
}

bool AMainLevelGameState::IsGameOver()
{
	if (CurrentGameTimer <= 0)
		return true;

	for (FTeamData team : TeamData)
		if (team.Kills == KillsToWin)
			return true;

	return false;
}

// I'm not in the mood to make this maintainable tonight.
EPlayerTeam AMainLevelGameState::GetWinningTeam()
{
	FTeamData pinkTeam = FTeamData();
	FTeamData cyanTeam = FTeamData();

	for (FTeamData team : TeamData)
	{
		if (team.Team == EPlayerTeam::Pink)
			pinkTeam = team;

		if (team.Team == EPlayerTeam::Cyan)
			cyanTeam = team;
	}

	if (pinkTeam.Kills > cyanTeam.Kills)
		return EPlayerTeam::Pink;

	else if (pinkTeam.Kills < cyanTeam.Kills)
		return EPlayerTeam::Cyan;

	return EPlayerTeam::None;
}

void AMainLevelGameState::StartEndTimer()
{
	IsEndCountingDown = true;
	CurrentEndTimer = DefaultEndTimer;

	Multi_OnResultsStart(WinningTeam);
}

void AMainLevelGameState::CountEndTimerDown(float DeltaTime)
{
	CurrentEndTimer -= DeltaTime;
	Multi_InvokeOnEndTimerChange();

	if (CurrentEndTimer <= 0)
	{
		IsEndCountingDown = false;

		if (HasAuthority())
			Server_ReturnToLobby();
	}
}

void AMainLevelGameState::IncreaseKillCount(EPlayerTeam team)
{
	for (FTeamData& t : TeamData)
	{
		if (t.Team == team)
		{
			t.Kills++;
			Multi_InvokeOnPlayerKill();
		}
	}
}

int AMainLevelGameState::GetTeamKillCount(EPlayerTeam team)
{
	int kills = 0;
	for (const FTeamData data : TeamData)
	{
		if (data.Team == team)
			kills = data.Kills;
	}

	return kills;
}

void AMainLevelGameState::Multi_InvokeOnSelectTimerChange_Implementation()
{
	OnSelectTimerChange.Broadcast();
}

void AMainLevelGameState::Multi_InvokeOnSelectTimerEnd_Implementation()
{
	OnSelectEnd.Broadcast();
}

void AMainLevelGameState::Multi_InvokeOnPlayerKill_Implementation()
{
	OnPlayerKill.Broadcast();
}

void AMainLevelGameState::Multi_InvokeOnMatchTimerChange_Implementation()
{
	OnMatchTimerChange.Broadcast();
	MatchTimerChange();
}

void AMainLevelGameState::Multi_InvokeOnEndTimerChange_Implementation()
{
	OnEndTimerChange.Broadcast();
}

void AMainLevelGameState::Multi_InvokeOnPlayerNameKill_Implementation(const AMainPlayerState* initiator, const AMainPlayerState* victim, const EWeapon& weapon, bool isHeadshot)
{
	OnPlayerNameKill.Broadcast(initiator, victim, weapon, isHeadshot);
}

void AMainLevelGameState::Server_ReturnToLobby_Implementation()
{
	Server_UnPossessAll();
	GetWorld()->ServerTravel(TravelMap, false, false);
}

void AMainLevelGameState::Server_SkipPreGame_Implementation()
{
	if (!GameSkipped)
	{
		CurrentSelectTimer = 0.0f;
		GameSkipped = true;
	}
}

FColor AMainLevelGameState::GetTeamColour(EPlayerTeam team)
{
	for (FTeamData data : TeamData)
	{
		if (data.Team == team)
			return data.Colour;
	}

	return FColor();
}

void AMainLevelGameState::MatchTimerChange_Implementation()
{
}

void AMainLevelGameState::StopFireAtEndGame_Implementation(APawn* Actor)
{
	APlayerMovement* PlayerMovement = Cast<APlayerMovement>(Actor);
	if (PlayerMovement && PlayerMovement->isShooting == true)
	{
		PlayerMovement->CurrentWeapon->StopFire();
	}
}

void AMainLevelGameState::OnPreGameStart_Implementation()
{
}

void AMainLevelGameState::OnMatchStart_Implementation()
{
}

void AMainLevelGameState::OnResultsStart_Implementation(const EPlayerTeam& winningTeam)
{
}

void AMainLevelGameState::Multi_OnPreGameStart_Implementation()
{
	OnPreGameStart();
}

void AMainLevelGameState::Multi_OnMatchStart_Implementation()
{
	OnMatchStart();
}

void AMainLevelGameState::Multi_OnResultsStart_Implementation(const EPlayerTeam& winningTeam)
{
	OnResultsStart(winningTeam);
}

void AMainLevelGameState::Server_UnPossessAll_Implementation()
{
	TArray<AActor*> pawns = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(this, APawn::StaticClass(), pawns);

	for (AActor* actor : pawns)
	{
		APawn* pawn = Cast<APawn>(actor);
		if (pawn && pawn->GetController())
		{
			StopFireAtEndGame(pawn);
			pawn->GetController()->UnPossess();
		}
	}
}

void AMainLevelGameState::Server_LoadEndScreen_Implementation(const EPlayerTeam& winningTeam)
{
	Multi_LoadEndScreen(winningTeam);
}

void AMainLevelGameState::Multi_LoadEndScreen_Implementation(const EPlayerTeam& winningTeam)
{
	APlayerController* controller = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
	if (controller)
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

		if (VictoryScreenTemplate)
		{
			UVictoryScreen* victory = CreateWidget<UVictoryScreen>(controller, VictoryScreenTemplate);
			if (victory)
			{
				victory->AddToViewport(5);

				OnEndTimerChange.AddDynamic(victory, &UVictoryScreen::OnTimerRefresh);
				victory->SetWinningTeam(winningTeam);
			}
		}

		else
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "VICTORY TEMPLATE CLASS NOT ASSIGNED");
	}
}