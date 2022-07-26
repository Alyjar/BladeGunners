#pragma once

#include "../GameModes/MainLevelGameMode.h"
#include "../RespawnSystem/RespawnPoint.h"
#include "../Pawns/SpectatorCameraPawn.h"
#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSController.generated.h"

UCLASS()
class SKATING_FPS_API AFPSController : public APlayerController
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Reliable)
	virtual void RespawnPawn();

	UFUNCTION(BlueprintCallable)
	void RequestCameraSpectator();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestCameraSpectator();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FTimerHandle RespawnHandle = FTimerHandle();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float RespawnDuration = 5.0f;

};
