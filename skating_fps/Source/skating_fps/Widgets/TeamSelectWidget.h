#pragma once

#include "../Enums/PlayerTeam.h"
#include "../Structs/TeamData.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectWidget.generated.h"

class AMainLevelGameState;
class ATeamPlayerState;
class UButton;
class UTextBlock;
class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamSelectSignature, const EPlayerTeam&, team);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpectatorSelectSignature);

UCLASS()
class SKATING_FPS_API UTeamSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable)
	void AssignUserNameTextBlock(EPlayerTeam team, UPanelWidget* parent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RequestSpawn(EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void RandomRequestSpawn();

	UFUNCTION(BlueprintCallable)
	void RequestSpectator();

	UFUNCTION(BlueprintCallable)
	int GetTeamNumber(EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void SetTextForTeam(UTextBlock* text, EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void SetBottomText(UTextBlock* text);

	UFUNCTION(BlueprintCallable)
	void SetNamesForTeam(EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void SetRandomButtonState(UButton* button);

	UFUNCTION(BlueprintCallable)
	void SetTeamButtonState(UButton* button, EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void SetSpectatorButtonState(UButton* button);

	UFUNCTION(BlueprintCallable)
	FTeamData GetTeamInfo(EPlayerTeam team);

	UFUNCTION(BlueprintCallable)
	void CloseWindow();

	UFUNCTION(BlueprintCallable)
	bool IsTeamFull(EPlayerTeam team);

private:
	UFUNCTION(BlueprintCallable)
	TArray<ATeamPlayerState*> GetPlayersOnTeam(EPlayerTeam team);

public:
	UPROPERTY(BlueprintAssignable)
	FTeamSelectSignature OnTeamSelect = FTeamSelectSignature();

	UPROPERTY(BlueprintAssignable)
	FSpectatorSelectSignature OnSpectatorSelect = FSpectatorSelectSignature();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TArray<FTeamData> Teams = {};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<UTextBlock*> CyanTeamNames = TArray<UTextBlock*>();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<UTextBlock*> PinkTeamNames = TArray<UTextBlock*>();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FString EmptyName = "-EMPTY-";

private:
	ATeamPlayerState* OwningTeamState = nullptr;
	AMainLevelGameState* LevelGameState = nullptr;
};
