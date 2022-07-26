#pragma once

#include "../Enums/PlayerTeam.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Scoreboard.generated.h"

class ATeamPlayerState;

UCLASS()
class SKATING_FPS_API UScoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void RefreshScoreboard();

protected:
	UFUNCTION(BlueprintCallable)
	TArray<ATeamPlayerState*> GetPlayersOnTeam(EPlayerTeam team);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCyanTeamButtons(const TArray<ATeamPlayerState*>& players);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPinkTeamButtons(const TArray<ATeamPlayerState*>& players);

	UFUNCTION(BlueprintCallable)
	void BubbleSort(TArray<ATeamPlayerState*>& orig);

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	FTimerHandle RefreshTimerHandle = FTimerHandle();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float RefereshTimerDuration = 0.25f;
};
