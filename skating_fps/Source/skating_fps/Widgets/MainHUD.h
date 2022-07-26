#pragma once

#include "../Enums/Weapon.h"

class UKillFeedBox;
class AMainLevelGameState;
class AMainPlayerState;
class UTextBlock;
class UVerticalBox;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

///////////////////////////////////////////////////////////////////////////////
// Reason I'm refreshing the values and not passing information through the
// delegate is because when this widget is remade - when the player dies -
// it will only update when the GameState requests a refresh.
///////////////////////////////////////////////////////////////////////////////
UCLASS()
class SKATING_FPS_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void RefreshKillCount();

	UFUNCTION(BlueprintCallable)
	void RefreshTimerText();

	UFUNCTION(BlueprintCallable)
	void AddKillToFeed(const AMainPlayerState* initiator, const AMainPlayerState* victim, const EWeapon& weapon, bool isHeadshot);

private:
	UFUNCTION(BlueprintCallable)
	AMainLevelGameState* GetMainGameState();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTextBlock* CyanKillCountText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTextBlock* PinkKillCountText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTextBlock* TimerText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UKillFeedBox> KillFeedTemplate = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UVerticalBox* KillFeedPanel = nullptr;
};
