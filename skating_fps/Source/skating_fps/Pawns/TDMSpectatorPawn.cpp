#include "TDMSpectatorPawn.h"

#include "../GameModes/MainLevelGameMode.h"

#include "Kismet/GameplayStatics.h"

ATDMSpectatorPawn::ATDMSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATDMSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATDMSpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATDMSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}