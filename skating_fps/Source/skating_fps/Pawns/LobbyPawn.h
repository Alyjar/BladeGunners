#pragma once

class ULobbyScreen;
class UTextChat;

#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPawn.generated.h"

UCLASS()
class SKATING_FPS_API ALobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	ALobbyPawn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_InitServerTravel();

	UFUNCTION(BlueprintCallable)
	void Disconnect();

	UFUNCTION(BlueprintCallable, Client, UnReliable)
	void Client_ForceDisconnect();

	UFUNCTION(BlueprintCallable, Client, UnReliable)
	void Client_RemoveWidget();

protected:
	virtual void BeginPlay() override;
	void InitialiseWidget();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<UWorld> DisconnectMap = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ULobbyScreen> LobbyScreenWdgt = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULobbyScreen* LobbyScreen = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextChat* TextChat = nullptr;
};
