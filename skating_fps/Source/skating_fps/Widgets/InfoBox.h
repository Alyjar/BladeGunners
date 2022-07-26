// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoBox.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SKATING_FPS_API UInfoBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInfoBox(const FObjectInitializer& ObjectInitializer);
	UInfoBox(const FObjectInitializer& ObjectInitializer, FString message);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetMessage(FString message);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTextBlock* InfoText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	FString Message = "";
};
