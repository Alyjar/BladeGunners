#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardPlayerInfo.generated.h"

class AMainPlayerState;

UCLASS()
class SKATING_FPS_API UScoreboardPlayerInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerByState(AMainPlayerState* playerstate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPlayerName(const FString& name);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsAlive(bool value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPlayerKills(int kills);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPlayerDeaths(int deaths);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPlayerPing(int ms);
};
