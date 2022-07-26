#pragma once

#include "../Enums/Weapon.h"

class AMainPlayerState;
class UTextBlock;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeedBox.generated.h"

UCLASS()
class SKATING_FPS_API UKillFeedBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInitiator(const AMainPlayerState* player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetVictim(const AMainPlayerState* player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetWeaponIcon(const EWeapon& weapon);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetHeadshotIcon(bool value);

	UFUNCTION(BlueprintCallable)
	void SetTextBoxValues(FString name, const AMainPlayerState* player, UTextBlock* text);

	UFUNCTION(BlueprintCallable)
	FString GetShortenedName(FString name);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int MaxNameLimit = 15;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FColor DefaultColour = FColor::White;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FColor CyanColour = FColor();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FColor PinkColour = FColor();
};
