// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyButton.h"

#include "Components/TextBlock.h"

/******************************************************************************
* Sets the value of the NameBlock.
******************************************************************************/
void ULobbyButton::SetNameText(FString str)
{
	if (NameBlock)
		NameBlock->SetText(FText::FromString(str));
}