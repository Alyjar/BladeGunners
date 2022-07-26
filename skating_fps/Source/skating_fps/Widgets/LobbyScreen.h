// Fillout your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Structs/PlayerChatMessage.h"
#include "Net/UnrealNetwork.h"
#include "Types/SlateEnums.h"

class ALobbyPawn;
class AMainPlayerState;
class UChatLine;
class UEditableTextBox;
class UScrollBox;
class ULobbyButton;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyScreen.generated.h"

UCLASS()
class SKATING_FPS_API ULobbyScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Overrides")
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void RefreshPlayerCards();
	UFUNCTION(BlueprintCallable)
	void RemovePlayerCards();

	UFUNCTION(BlueprintCallable)
	void SetButtonsActiveState(ESlateVisibility vis);

	UFUNCTION(BlueprintCallable)
	void AppendMessage(const FPlayerChatMessage& message);

	UFUNCTION(BlueprintCallable)
	void PopMessage();

	UFUNCTION(BlueprintCallable)
	void SendMessage(UEditableTextBox* input, FText text, ETextCommit::Type commit);

private:
	void InitialiseButtons(int max = 8);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"))
	ALobbyPawn* Owner = nullptr;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UScrollBox* ChatScrollBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UChatLine> ChatLineTemplate = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<ULobbyButton*> PlayerButtons = TArray<ULobbyButton*>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ULobbyButton> LobbyButtonTemplate = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UScrollBox* PlayerScrollBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RefreshDelay = 0.25f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float _RefreshDelay = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int MaxMessages = 40;
};
