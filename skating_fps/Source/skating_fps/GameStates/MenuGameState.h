// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UUserWidget;
class InfoBox;

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MenuGameState.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API AMenuGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void LoadLastErrorMessage();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InfoBoxBP = nullptr;
};