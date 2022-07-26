#pragma once

#include "../Enums/PlayerTeam.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryScreen.generated.h"

class ATeamPlayerState;

UCLASS()
class SKATING_FPS_API UVictoryScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWinningTeam(const EPlayerTeam& team);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTimerRefresh();

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTie();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnCyanWin();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnPinkWin();

	UFUNCTION(BlueprintNativeEvent)
	void OnWin();

	UFUNCTION(BlueprintNativeEvent)
	void OnLose();

	UFUNCTION(BlueprintCallable)
	bool HasWon();

	UFUNCTION(BlueprintCallable)
	void SetMVPs();
	UFUNCTION(BlueprintCallable)
	ATeamPlayerState* GetMVP(EPlayerTeam team);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCyanMVP(const FString& name, int Kills);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetPinkMVP(const FString& name, int Kills);

	UFUNCTION(BlueprintCallable)
	TArray<ATeamPlayerState*> GetPlayersOnTeam(EPlayerTeam team);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EPlayerTeam WinningTeam = EPlayerTeam::None;

	UPROPERTY(EditAnywhere, BlueprintReadwRITE, meta = (AllowPrivateAccess = true))
	FString NullName = "None";
};
