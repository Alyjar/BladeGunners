#include "LobbyPawn.h"

#include "../Components/TextChat.h"
#include "../GameModes/LobbyGameMode.h"
#include "../PlayerStates/MainPlayerState.h"
#include "../Widgets/LobbyScreen.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../GameInstances/SteamGlobalGameInstance.h"

ALobbyPawn::ALobbyPawn()
{
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = true;

	TextChat = CreateDefaultSubobject<UTextChat>(TEXT("Text Chat"));
	this->AddOwnedComponent(TextChat);
}

void ALobbyPawn::Server_InitServerTravel_Implementation()
{
	ALobbyGameMode* gm = Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gm)
		gm->ServerTravel();
}

void ALobbyPawn::Disconnect()
{
	if (HasAuthority())
	{
		TArray<AActor*> pawns = {};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), this->StaticClass(), pawns);
		for (AActor* actor : pawns)
		{
			ALobbyPawn* pawn = Cast<ALobbyPawn>(actor);
			pawn->Client_ForceDisconnect();
		}
	}

	USteamGlobalGameInstance* ins = Cast<USteamGlobalGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (ins)
	{
		ins->Disconnect();
		ins->LoadMap(DisconnectMap);
	}
}

void ALobbyPawn::Client_ForceDisconnect_Implementation()
{
	if (!HasAuthority())
	{
		Disconnect();

		USteamGlobalGameInstance* ins = Cast<USteamGlobalGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (ins)
			ins->LoadMapWithFailMessage(DisconnectMap, "SERVER CONNECTION LOST");
	}
}

void ALobbyPawn::Client_RemoveWidget_Implementation()
{
	if (LobbyScreen)
		LobbyScreen->RemoveFromParent();
}

void ALobbyPawn::BeginPlay()
{
	Super::BeginPlay();

	InitialiseWidget();

	if (LobbyScreen && IsLocallyControlled())
		TextChat->OnMessageReceived.AddDynamic(LobbyScreen, &ULobbyScreen::AppendMessage);
	
	APlayerController* controller = Cast<APlayerController>(GetController());
	if (controller)
	{
		controller->SetInputMode(FInputModeUIOnly());
		controller->bShowMouseCursor = true;
	}
}

void ALobbyPawn::InitialiseWidget()
{
	if (IsLocallyControlled())
	{
		LobbyScreen = CreateWidget<ULobbyScreen>(GetWorld(), LobbyScreenWdgt);
		LobbyScreen->Owner = this;

		LobbyScreen->AddToViewport();
		APlayerController* controller = Cast<APlayerController>(GetController());
		if (controller)
		{
			controller->SetInputMode(FInputModeUIOnly());
			controller->bShowMouseCursor = false;
		}
	}
}

void ALobbyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALobbyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

