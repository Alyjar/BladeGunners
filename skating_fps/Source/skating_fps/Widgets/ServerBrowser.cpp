#include "ServerBrowser.h"

#include "Engine/LocalPlayer.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Net/OnlineEngineInterface.h"
#include "SessionButton.h"


void UServerBrowser::NativeConstruct()
{
	Super::NativeConstruct();

	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UServerBrowser::OnLocalSearchResultsComplete);

	InstantiateAllButtons();
	//SearchForLocalSessions();
}

// Source: https://unreal.gg-labs.com/wiki-archives/networking/how-to-use-sessions-in-c++
void UServerBrowser::SearchForLocalSessions()
{
	IOnlineSubsystem* online = IOnlineSubsystem::Get();
	if (online)
	{
		RefreshCircle->SetVisibility(ESlateVisibility::Visible);

		IOnlineSessionPtr sessions = online->GetSessionInterface();
		OnlineResults = MakeShareable(new FOnlineSessionSearch());
		OnlineResults->bIsLanQuery = true;
		OnlineResults->MaxSearchResults = MaxLobbiesShown;
		OnlineResults->PingBucketSize = 250;

		TSharedRef<FOnlineSessionSearch> searchSettingRef = OnlineResults.ToSharedRef();

		ULocalPlayer* player = GetOwningLocalPlayer();
		TSharedPtr<const FUniqueNetId> userID = player->GetPreferredUniqueNetId().GetUniqueNetId();

		OnFindSessionsCompleteDelegateHandle = sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
		sessions->FindSessions(*userID, searchSettingRef);
	}

	else
		OnLocalSearchResultsComplete(false);
}

void UServerBrowser::OnLocalSearchResultsComplete(bool success)
{
	IOnlineSubsystem* online = IOnlineSubsystem::Get();
	if (online)
	{
		IOnlineSessionPtr sessions = online->GetSessionInterface();
		sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

		// Adds the dedicated servers first then the local ones.
		TArray<FServerInfo> servers = TArray<FServerInfo>(PresetServers);
		servers.Append(ExtrapolateServerInfo(OnlineResults->SearchResults));

		SetSessions(servers);
	}

	RefreshCircle->SetVisibility(ESlateVisibility::Hidden);
}

TArray<FServerInfo> UServerBrowser::ExtrapolateServerInfo(TArray<FOnlineSessionSearchResult> results)
{
	TArray<FServerInfo> servers = TArray<FServerInfo>();
	IOnlineSubsystem* online = IOnlineSubsystem::Get();
	if (online)
	{
		IOnlineSessionPtr sessions = online->GetSessionInterface();
		for (FOnlineSessionSearchResult result : results)
		{
			FString ip = "";
			sessions.Get()->GetResolvedConnectString(result, NAME_GamePort, ip);

			FServerInfo info = FServerInfo(result.Session.OwningUserName, ip);
			servers.Add(info);
		}
	}

	return servers;
}

/******************************************************************************
* Sets the session results available to the Player. It also displays these
* results as buttons that are selectable.
******************************************************************************/
void UServerBrowser::SetSessions(TArray<FServerInfo> servers)
{
	if (!ScrollBox)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "ASSIGN THE SCROLLBOX INSTANCE IN BLUEPRINTS");
		return;
	}

	for (int i = 0; i < servers.Num(); i++)
	{
		USessionButton* btnWidget = SessionsAvailable[i];
		btnWidget->Server = servers[i];
		btnWidget->SetGraphics();
		btnWidget->SetVisibility(ESlateVisibility::Visible);	
	}
}

/******************************************************************************
* Updates the Details panel based on what Button has been selected.
******************************************************************************/
void UServerBrowser::SetSessionDetails_Implementation(const FServerInfo& info)
{
	SelectedServer = info;
}

void UServerBrowser::JoinSession()
{	
	FString ip = SelectedServer.ServerIP;
	if (ip.Len() > 0)
		GetOwningLocalPlayer()->GetPlayerController(GetWorld())->ClientTravel(ip, ETravelType::TRAVEL_Absolute);
}

/******************************************************************************
* Spawns all of the Buttons at the Start to Object Pool.
******************************************************************************/
void UServerBrowser::InstantiateAllButtons()
{
	if (!SessionButtonTemplate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "ASSIGN THE BUTTON TEMPLATE");
		return;
	}

	for (int i = 0; i < MaxLobbiesShown; i++)
	{
		USessionButton* btnWidget = CreateWidget<USessionButton>(ScrollBox, SessionButtonTemplate, TEXT("BUTTON " + i));
		ScrollBox->AddChild(btnWidget);
		SessionsAvailable.Add(btnWidget);

		btnWidget->OnClickedInfo.AddDynamic(this, &UServerBrowser::SetSessionDetails);
	}

	SetButtonsActiveState(ESlateVisibility::Collapsed);
}

/******************************************************************************
* Sets the state of the Sessions Available Buttons.
******************************************************************************/
void UServerBrowser::SetButtonsActiveState(ESlateVisibility vis)
{
	for (USessionButton* button : SessionsAvailable)
	{
		if (button)
			button->SetVisibility(vis);
	}
}