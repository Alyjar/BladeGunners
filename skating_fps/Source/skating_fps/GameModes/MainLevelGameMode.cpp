#include "MainLevelGameMode.h"

#include "../Controllers/TDMController.h"
#include "../GameStates/MainLevelGameState.h"
#include "../Health/Health.h"
#include "../PlayerStates/TeamPlayerState.h"
#include "../Pawns/TDMSpectatorPawn.h"

#include "TimerManager.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AMainLevelGameMode::AMainLevelGameMode() 
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MediocreShooter/Characters/Player_BP"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// I don't want the Pawn to spawn immediately.
	bDelayedStart = true;
}

void AMainLevelGameMode::BeginPlay()
{
	Super::BeginPlay();

	UClass* SpawnPointClass = ARespawnPoint::StaticClass();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpectatorCameraPawn::StaticClass(), cameras);

	AMainLevelGameState* gs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (gs)
		gs->OnSelectEnd.AddDynamic(this, &AMainLevelGameMode::RequestReadyPlayerSpawn);
}

void AMainLevelGameMode::SpectatorSlotRequested(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		FActorSpawnParameters params = FActorSpawnParameters();
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ARespawnPoint* respawnPoint = GetSpawnPoint();
		FVector respawnLocation = respawnPoint ? respawnPoint->GetActorLocation() : FVector::ZeroVector;
		FRotator respawnRotation = respawnPoint ? respawnPoint->GetActorRotation() : FRotator::ZeroRotator;

		respawnLocation.Z += SpectatorZOffset;

		if (SpectatorClass)
		{
			APawn* spectator = GetWorld()->SpawnActor<APawn>(SpectatorClass, respawnLocation, respawnRotation, params);

			if (spectator)
			{
				PlayerController->Possess(spectator);
			}
		}
	}
}

void AMainLevelGameMode::CameraSpectatorRequested(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (CameraSpectatorTemplate)
		{
			FActorSpawnParameters params = FActorSpawnParameters();
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ASpectatorCameraPawn* pawn = GetWorld()->SpawnActor<ASpectatorCameraPawn>(CameraSpectatorTemplate, FVector::ZeroVector, FRotator::ZeroRotator, params);
			if (pawn)
			{
				PlayerController->Possess(pawn);
				pawn->Client_CreateHUD();
			}
		}

		else
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "NO CAMERA SPECTATOR TEMPLATE ASSIGNED!");
	}
}

void AMainLevelGameMode::AddPlayerToQueue(APlayerController* controller)
{
	if (!PlayersToSpawn.Contains(controller))
		PlayersToSpawn.Add(controller);
}

void AMainLevelGameMode::RequestReadyPlayerSpawn()
{
	for (APlayerController* controller : PlayersToSpawn)
		RespawnRequested(controller);

	PlayersToSpawn.Empty();
}

ARespawnPoint* AMainLevelGameMode::GetSpawnPoint()
{
	TArray<AActor*> spawnpoints = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARespawnPoint::StaticClass(), spawnpoints);

	if (spawnpoints.Num() <= 0)
		return nullptr;

	int random = FMath::RandRange(0, spawnpoints.Num() - 1);
	return Cast<ARespawnPoint>(spawnpoints[random]);
}

ASpectatorCameraPawn* AMainLevelGameMode::GetCameraPoint(int CameraID)
{
	if (CameraID < 0 || CameraID > cameras.Num())
		return nullptr;

	return Cast<ASpectatorCameraPawn>(cameras[CameraID]);
}


void AMainLevelGameMode::RespawnRequested_Implementation(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		// In the Team Select stage.
		if (!IsMatchReady())
			AddPlayerToQueue(PlayerController);

		else if (ARespawnPoint* SpawnPoint = GetSpawnPoint())
		{
			FVector Location = SpawnPoint->GetActorLocation();
			FRotator Rotation = SpawnPoint->GetActorRotation();
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation, SpawnParameters))
				PlayerController->Possess(Pawn);
		}
	}
}

void AMainLevelGameMode::SpawnPlayer_Implementation(EPlayerTeam team, APlayerController* controller)
{
	// This is overwritten in the BP. It is here where the Team is assigned to the playerstate.
	// TODO is to make this C++.
	// Wait this isn't used...
}

void AMainLevelGameMode::SetPlayerTeam(ATeamPlayerState* playerstate, EPlayerTeam team)
{
	if (playerstate)
	{
		FTeamData data = GetTeamData(team);

		playerstate->Team = data;
		playerstate->Client_CloseTeamSelectWindow();

		OnTeamChangeDelegate.Broadcast(playerstate);
	}
}

FTeamData AMainLevelGameMode::GetTeamData(EPlayerTeam team)
{
	// I would use a cached reference, but I can't make it a reference to the GameState.
	// I would prefer the cost of performance and the benfit of being auto-updated.
	AMainLevelGameState* mgs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (mgs)
	{
		for (FTeamData data : mgs->TeamData)
		{
			if (data.Team == team)
				return data;
		}
	}
	

	return FTeamData();
}

bool AMainLevelGameMode::IsMatchReady()
{
	AMainLevelGameState* gs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (gs)
		return gs->IsGameReady();

	return false;
}
