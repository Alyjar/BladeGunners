// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UTextBlock;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyButton.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API ULobbyButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// Sets the Text of the NameBlock.
	UFUNCTION(BlueprintCallable, Category = "Text")
	void SetNameText(FString str);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Text", meta = (AllowPrivateAccess = "true"))
	UTextBlock* NameBlock = nullptr;
};
