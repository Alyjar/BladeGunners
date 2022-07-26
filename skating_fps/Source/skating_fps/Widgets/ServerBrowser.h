// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Structs/ServerInfo.h"

#include "FindSessionsCallbackProxy.h"
#include "Online.h"
#include "OnlineSubsystem.h"

class UCircularThrobber;
class UScrollBox;
class USessionButton;
class UTextBlock;

#include "CoreMinimal.h"
#include "../Widgets/PopUpWindow.h"
#include "ServerBrowser.generated.h"

UCLASS()
class SKATING_FPS_API UServerBrowser : public UPopUpWindow
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Overrides")
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SearchForLocalSessions();

	UFUNCTION()
	void OnLocalSearchResultsComplete(bool success);

	// Can't be a UFUNCTION since the paramater data type isn't a UCLASS smh.
	TArray<FServerInfo> ExtrapolateServerInfo(TArray<FOnlineSessionSearchResult> results);

	/// Displays the available Sessions to join.
	UFUNCTION(BlueprintCallable, Category = "Networking")
	void SetSessions(TArray<FServerInfo> servers);

	/// Shows the detail of Session selected.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Networking")
	void SetSessionDetails(const FServerInfo& info);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	void JoinSession();

	/// Object Pools the Server Buttons.
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void InstantiateAllButtons();

	/// Sets the state of the Server Buttons.
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void SetButtonsActiveState(ESlateVisibility vis);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USessionButton> SessionButtonTemplate;

private:
	TSharedPtr<FOnlineSessionSearch> OnlineResults = nullptr;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking", meta = (AllowPrivateAccess = "true"))
	TArray<FServerInfo> PresetServers = TArray<FServerInfo>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking", meta = (AllowPrivateAccess = "true"))
	FServerInfo SelectedServer = FServerInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking", meta = (AllowPrivateAccess = "true"))
	int MaxLobbiesShown = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Session", meta = (AllowPrivateAccess = "true"))
	TArray<USessionButton*> SessionsAvailable = {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCircularThrobber* RefreshCircle = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UScrollBox* ScrollBox = nullptr;
};
