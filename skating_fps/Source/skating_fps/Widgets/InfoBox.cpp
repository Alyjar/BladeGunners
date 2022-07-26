// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoBox.h"

#include "Components/TextBlock.h"

UInfoBox::UInfoBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Message = "";
}

UInfoBox::UInfoBox(const FObjectInitializer& ObjectInitializer, FString message) : Super(ObjectInitializer)
{
	Message = message;
}

void UInfoBox::NativeConstruct()
{
	SetMessage(Message);

	Super::NativeConstruct();
}

void UInfoBox::SetMessage(FString message)
{
	Message = message;
	InfoText->SetText(FText::FromString(message));
}