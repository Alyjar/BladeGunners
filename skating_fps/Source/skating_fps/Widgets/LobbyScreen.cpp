// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyScreen.h"

#include "../Components/TextChat.h"
#include "../Pawns/LobbyPawn.h"
#include "../PlayerStates/MainPlayerState.h"
#include "../Widgets/ChatLine.h"
#include "../Widgets/LobbyButton.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ULobbyScreen::NativeConstruct()
{
	Super::NativeConstruct();
	InitialiseButtons();
}

void ULobbyScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	_RefreshDelay -= InDeltaTime;
	if (_RefreshDelay <= 0.0f)
	{
		RefreshPlayerCards();
		_RefreshDelay = RefreshDelay;
	}

	// Supposedly better since if you call Tick in BP it does it first otherwise.
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULobbyScreen::RefreshPlayerCards()
{
	SetButtonsActiveState(ESlateVisibility::Collapsed); 

	// An nullptr would occur with a server travel otherwise.
	if (!GetWorld() || !UGameplayStatics::GetGameState(GetWorld()))
		return; 

	const TArray<APlayerState*>& players = UGameplayStatics::GetGameState(GetWorld())->PlayerArray;
	for (int i = 0; i < players.Num(); i++)
	{
		if (i < PlayerButtons.Num() && PlayerButtons[i]) 
		{
			ULobbyButton* btn = PlayerButtons[i];
			btn->SetVisibility(ESlateVisibility::Visible);

			AMainPlayerState* mps = Cast<AMainPlayerState>(players[i]);
			if (mps)
				btn->SetNameText(mps->UserName);
		}
	}
}

void ULobbyScreen::RemovePlayerCards()
{
	// UE4 won't allow for initialisation of a pointer for getting a child.
	for (int i = 0; i < PlayerScrollBox->GetChildrenCount(); i++)
		PlayerScrollBox->GetChildAt(i)->RemoveFromParent();
}

void ULobbyScreen::SetButtonsActiveState(ESlateVisibility vis)
{
	for (ULobbyButton* button : PlayerButtons)
	{
		if (button)
			button->SetVisibility(vis);
	}
}

void ULobbyScreen::AppendMessage(const FPlayerChatMessage& message)
{
	if (ChatScrollBox && ChatLineTemplate)
	{
		UChatLine* chat = CreateWidget<UChatLine>(ChatScrollBox, ChatLineTemplate);
		ChatScrollBox->AddChild(chat);
		ChatScrollBox->ScrollToEnd();
		chat->SetText(message.UserName + ": " + message.Message);

		if (ChatScrollBox->GetChildrenCount() > MaxMessages)
			PopMessage();
	}
}

void ULobbyScreen::PopMessage()
{
	if (ChatScrollBox && ChatScrollBox->GetChildrenCount() > 0)
	{
		UWidget* child = ChatScrollBox->GetChildAt(0);
		if (child)
			child->RemoveFromParent();
	}
}

void ULobbyScreen::SendMessage(UEditableTextBox* input, FText text, ETextCommit::Type commit)
{
	if (commit == ETextCommit::OnEnter && input)
	{
		input->SetText(FText());

		AMainPlayerState* mps = GetOwningPlayer()->GetPlayerState<AMainPlayerState>();
		if (mps && Owner && Owner->TextChat)
		{
			FPlayerChatMessage message = FPlayerChatMessage(mps->UserName, text.ToString());
			Owner->TextChat->Server_SendMessage(message);
		}
	}
}

void ULobbyScreen::InitialiseButtons(int max)
{
	if (!PlayerScrollBox)
		return;


	for (int i = 0; i < max; i++)
	{
		ULobbyButton* btn = CreateWidget<ULobbyButton>(PlayerScrollBox, LobbyButtonTemplate, TEXT("BUTTON " + i));
		PlayerButtons.Add(btn);

		PlayerScrollBox->AddChild(btn);
	}

	SetButtonsActiveState(ESlateVisibility::Collapsed);
}
