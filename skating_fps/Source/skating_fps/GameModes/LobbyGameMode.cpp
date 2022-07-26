#include "LobbyGameMode.h"

#include "../Pawns/LobbyPawn.h"

#include "Kismet/GameplayStatics.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = false;
}

void ALobbyGameMode::ServerTravel()
{
	TArray<AActor*> pawns = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPawn::StaticClass(), pawns);
	for (AActor* actor : pawns)
	{
		ALobbyPawn* pawn = Cast<ALobbyPawn>(actor);
		if (pawn)
			pawn->Client_RemoveWidget();
	}

	// Absolute is off to allow Clients to connect correctly.
	// SEAMLESS TRAVEL IS NOW DISABLED.
	// This is to ensure that Playerstates and GameModes are disabled.
	GetWorld()->ServerTravel(TravelMap, false, false);
}