#include "FPSController.h"

#include "../PlayerStates/MainPlayerState.h"

#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetwork.h"

void AFPSController::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFPSController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AFPSController::RespawnPawn_Implementation()
{
	// The implementation is only supported for TDM atm.
	// Hence it's in the TDMController.
}

void AFPSController::RequestCameraSpectator()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, this, &AFPSController::RespawnPawn, RespawnDuration, false);
		Server_RequestCameraSpectator();
	}
}

void AFPSController::Server_RequestCameraSpectator_Implementation()
{
	AMainLevelGameMode* gm = Cast<AMainLevelGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
		gm->CameraSpectatorRequested(this);
}
