#pragma once

#include "../Structs/TeamData.h"
#include "../Enums/PlayerTeam.h"
#include "../Enums/Weapon.h"

#include "Net/UnrealNetwork.h"

class APlayerController;
class APawn;
class AMainPlayerState;
class UVictoryScreen;

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainLevelGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FKillSignature, const AMainPlayerState*, Initiator, const AMainPlayerState*, Victim, const EWeapon&, weapon, bool, wasHeadshot);

UCLASS()
class SKATING_FPS_API AMainLevelGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMainLevelGameState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Pre-Game
	UFUNCTION(BlueprintCallable)
	void StartSelectTimer();

	UFUNCTION(BlueprintCallable)
	void CountSelectTimerDown(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool IsGameReady();

	UFUNCTION(BlueprintCallable)
	EPlayerTeam GetWinningTeam();

	// Match Duration
	UFUNCTION(BlueprintCallable)
	void StartMatchTimer();

	UFUNCTION(BlueprintCallable)
	void CountMatchTimerDown(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool IsGameOver();


	// End Game
	UFUNCTION(BlueprintCallable)
	void StartEndTimer();

	UFUNCTION(BlueprintCallable)
	void CountEndTimerDown(float DeltaTime);


	UFUNCTION(BlueprintCallable)
	void IncreaseKillCount(EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	int GetTeamKillCount(EPlayerTeam team);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnPlayerKill();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnSelectTimerChange();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnSelectTimerEnd();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnMatchTimerChange();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnEndTimerChange();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multi_InvokeOnPlayerNameKill(const AMainPlayerState* initiator, const AMainPlayerState* victim, const EWeapon& weapon, bool isHeadshot);

	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_ReturnToLobby();

	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_SkipPreGame();

	UFUNCTION(BlueprintCallable)
	FColor GetTeamColour(EPlayerTeam team);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnPreGameStart();
	UFUNCTION(BlueprintNativeEvent)
	void OnMatchStart();
	UFUNCTION(BlueprintNativeEvent)
	void OnResultsStart(const EPlayerTeam& winningTeam);
	UFUNCTION(BlueprintNativeEvent)
	void MatchTimerChange();

private:
	UFUNCTION(NetMulticast, UnReliable)
	void Multi_OnPreGameStart();
	UFUNCTION(NetMulticast, UnReliable)
	void Multi_OnMatchStart();
	UFUNCTION(NetMulticast, UnReliable)
	void Multi_OnResultsStart(const EPlayerTeam& winningTeam);

	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_UnPossessAll();
	UFUNCTION(NetMulticast, Reliable)
	void StopFireAtEndGame(APawn* pawn);

	UFUNCTION(Server, BlueprintCallable, Reliable)
	void Server_LoadEndScreen(const EPlayerTeam& winningTeam);

	UFUNCTION(NetMulticast, BlueprintCallable, Reliable)
	void Multi_LoadEndScreen(const EPlayerTeam& winningTeam);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int KillsToWin = 50;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FRefreshSignature OnPlayerKill = FRefreshSignature();

	UPROPERTY(BlueprintAssignable)
	FRefreshSignature OnSelectTimerChange = FRefreshSignature();

	UPROPERTY(BlueprintAssignable)
	FRefreshSignature OnSelectEnd = FRefreshSignature();

	UPROPERTY(BlueprintAssignable)
	FRefreshSignature OnMatchTimerChange = FRefreshSignature();

	UPROPERTY(BlueprintAssignable)
	FRefreshSignature OnEndTimerChange = FRefreshSignature();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FKillSignature OnPlayerNameKill = FKillSignature();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	TArray<FTeamData> TeamData = TArray<FTeamData>();


	////////////////////////////////////////////////////////////////////////////////
	// In Hindsight I would have probably maintained this better.
	////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float DefaultSelectTimer = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float CurrentSelectTimer = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool IsSelectCountingDown = false;

	// Seconds * Minutes wanted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float DefaultGameTimer = 60.0f * 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float CurrentGameTimer = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool IsMatchCountingDown = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float DefaultEndTimer = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float CurrentEndTimer = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool IsEndCountingDown = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString TravelMap = "Lobby_MAP";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UVictoryScreen> VictoryScreenTemplate = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool GameSkipped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool BeginPlayCalled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerTeam WinningTeam = EPlayerTeam::None;
};
