#include "TDMController.h"

#include "../FunctionLibraries/General.h"
#include "../GameModes/MainLevelGameMode.h"
#include "../GameStates/MainLevelGameState.h"
#include "../RespawnSystem/RespawnPlayer.h"
#include "../PlayerStates/TeamPlayerState.h"
#include "../Widgets/TeamSelectWidget.h"

#include "Kismet/GameplayStatics.h"

void ATDMController::BeginPlay()
{
	Super::BeginPlay();

	OpenTeamSelect();

	// This is mainly for when you select a character and are waiting
	// for the game to start.
	AMainLevelGameState* gs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (gs)
	{
		gs->OnSelectEnd.AddDynamic(this, &ATDMController::CloseTeamSelect);
		MainGameState = gs;
	}
}

void ATDMController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (MainGameState && IsInputKeyDown(EKeys::LeftAlt))
		Server_SkipPreGame();
}

void ATDMController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SwapTeams", IE_Pressed, this, &ATDMController::ToggleTeamSelect);
}

void ATDMController::ToggleTeamSelect()
{
	if (TeamSelectScreen)
		CloseTeamSelect();

	else
		OpenTeamSelect();
}

void ATDMController::OpenTeamSelect()
{
	if (!UGeneral::IsOwnedBy(this, this))
		return;

	if (IsLocalPlayerController() && TeamSelectTemplate && !TeamSelectScreen)
	{
		TeamSelectScreen = CreateWidget<UTeamSelectWidget>(GetWorld(), TeamSelectTemplate);
		TeamSelectScreen->AddToViewport();

		TeamSelectScreen->OnTeamSelect.AddDynamic(this, &ATDMController::RequestRespawn);
		TeamSelectScreen->OnSpectatorSelect.AddDynamic(this, &ATDMController::RequestSpectator);

		bShowMouseCursor = true;
		EnableInput(this);
	}

	else if (!TeamSelectTemplate)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "NO TEAMSELECT TEMPLATE CHOSEN!");
}

void ATDMController::CloseTeamSelect()
{
	ATeamPlayerState* tps = GetPlayerState<ATeamPlayerState>();
	if (!UGeneral::IsOwnedBy(this, this) || (tps && tps->Team.Team == EPlayerTeam::None))
		return;

	ForceCloseTeamSelect();
}

void ATDMController::ForceCloseTeamSelect()
{
	AMainLevelGameState* mgs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (mgs && !mgs->IsGameReady())
		return;

	if (TeamSelectScreen)
	{
		TeamSelectScreen->RemoveFromViewport();
		TeamSelectScreen = nullptr;

		bShowMouseCursor = false;
	}
}

void ATDMController::DestroyPawn()
{
	if (GetPawn())
	{
		URespawnPlayer* respawn = GetPawn()->FindComponentByClass<URespawnPlayer>();
		if (respawn)
		{
			respawn->Death();

			// TODO: Make it so that it uses the respawn time!
			//respawn->RespawnDelay();
		}

		// Worst Case if the Component is removed.
		// Or if it is the spectator
		else
			GetPawn()->Destroy();
	}
}

void ATDMController::RequestRespawn(const EPlayerTeam& team)
{
	Server_RequestRespawn(team);
}

void ATDMController::Server_RequestRespawn_Implementation(EPlayerTeam team)
{
	DestroyPawn();

	AMainLevelGameMode* gm = Cast<AMainLevelGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->SetPlayerTeam(GetPlayerState<ATeamPlayerState>(), team);
		gm->RespawnRequested(this);
	}
}

void ATDMController::RespawnPawn()
{
	Super::RespawnPawn();

	ATeamPlayerState* ps = Cast<ATeamPlayerState>(PlayerState);
	if (ps)
		RequestRespawn(ps->Team.Team);
}

void ATDMController::RequestSpectator()
{
	Server_RequestSpectator();
}

void ATDMController::Server_RequestSpectator_Implementation()
{
	DestroyPawn();

	AMainLevelGameMode* gm = Cast<AMainLevelGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
	{
		gm->SetPlayerTeam(GetPlayerState<ATeamPlayerState>(), EPlayerTeam::Spectator);
		gm->SpectatorSlotRequested(this);
	}
}

void ATDMController::Server_SkipPreGame_Implementation()
{
	if (MainGameState)
		MainGameState->Server_SkipPreGame();
}