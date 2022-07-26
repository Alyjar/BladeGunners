#pragma once

#include "GameFramework/PlayerInput.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenu.generated.h"

UCLASS(Config=Game)
class SKATING_FPS_API UOptionsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintCallable)
	void SetMasterSound(float value);
	UFUNCTION(BlueprintCallable)
	float GetMasterSound();

	UFUNCTION(BlueprintCallable)
	void SetMusicSound(float value);
	UFUNCTION(BlueprintCallable)
	float GetMusicSound();

	UFUNCTION(BlueprintCallable)
	void SetVFXSound(float value);
	UFUNCTION(BlueprintCallable)
	float GetVFXSound();

	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float value);

	UFUNCTION(BlueprintCallable)
	void SetMouseXInput(bool isInverted);
	UFUNCTION(BlueprintCallable)
	void SetMouseYInput(bool isInverted);

	UFUNCTION(BlueprintCallable)
	float GetMouseSensitivity();

	UFUNCTION(BlueprintCallable)
	bool IsMouseXInverted();
	UFUNCTION(BlueprintCallable)
	bool IsMouseYInverted();

	UFUNCTION(BlueprintCallable)
	FIntPoint GetResolutionFromString(FString resolution);

private:
	FInputAxisKeyMapping GetFirstAxisMapping(FString value);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString MouseXName = "Turn";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString MouseYName = "LookUp";

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float MouseSensitivity = 1.0f;
};
