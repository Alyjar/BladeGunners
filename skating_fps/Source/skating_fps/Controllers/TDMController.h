#pragma once

#include "CoreMinimal.h"
#include "FPSController.h"
#include "TDMController.generated.h"

class AMainLevelGameState;
class ATeamPlayerState;
class UTeamSelectWidget;

UCLASS()
class SKATING_FPS_API ATDMController : public AFPSController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void BeginPlay() override;

	// PlayerTick is only Client.
	// Tick is both.
	UFUNCTION(BlueprintCallable)
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ToggleTeamSelect();

	UFUNCTION(BlueprintCallable)
	void OpenTeamSelect();

	UFUNCTION(BlueprintCallable)
	void CloseTeamSelect();

	UFUNCTION(BlueprintCallable)
	void ForceCloseTeamSelect();

	UFUNCTION(BlueprintCallable)
	void DestroyPawn();

	UFUNCTION(BlueprintCallable)
	void RequestRespawn(const EPlayerTeam& team);

	UFUNCTION(Server, Reliable)
	void Server_RequestRespawn(EPlayerTeam team);

	virtual void RespawnPawn() override;

	UFUNCTION(BlueprintCallable)
	void RequestSpectator();

	UFUNCTION(Server, Reliable)
	void Server_RequestSpectator();

private:
	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_SkipPreGame();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UTeamSelectWidget> TeamSelectTemplate = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UTeamSelectWidget* TeamSelectScreen = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AMainLevelGameState* MainGameState = nullptr;
};
