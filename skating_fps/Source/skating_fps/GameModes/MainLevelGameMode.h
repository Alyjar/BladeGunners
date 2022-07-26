#pragma once

#include "../Enums/PlayerTeam.h"
#include "../RespawnSystem/RespawnPoint.h"
#include "../Pawns/SpectatorCameraPawn.h"
#include "../Structs/TeamData.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainLevelGameMode.generated.h"

class APlayerController;
class ARespawnPoint;
class ASpectatorCameraPawn;
class ATeamPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChange, ATeamPlayerState*, playerstate);

UCLASS()
class SKATING_FPS_API AMainLevelGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMainLevelGameMode();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpectatorSlotRequested(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void CameraSpectatorRequested(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void AddPlayerToQueue(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
	void RequestReadyPlayerSpawn();

	UFUNCTION(Server, Reliable)
	void RespawnRequested(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	ARespawnPoint* GetSpawnPoint();

	UFUNCTION(BlueprintCallable)
	ASpectatorCameraPawn* GetCameraPoint(int CameraID);

	UFUNCTION(BlueprintNativeEvent, meta = (DeprecatedFunction, DeprecationMessage = "Use RespawnRequested Instead."))
	void SpawnPlayer(EPlayerTeam team, APlayerController* controller);

	UFUNCTION(BlueprintCallable)
	void SetPlayerTeam(ATeamPlayerState* playerstate, EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	FTeamData GetTeamData(EPlayerTeam team);

protected:
	UFUNCTION(BlueprintCallable)
	bool IsMatchReady();

public:
	TArray<AActor*> cameras;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnTeamChange OnTeamChangeDelegate = FOnTeamChange();

private:
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<APlayerController*> PlayersToSpawn = TArray<APlayerController*>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ASpectatorCameraPawn> CameraSpectatorTemplate = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float SpectatorZOffset = 100.0f;
};
