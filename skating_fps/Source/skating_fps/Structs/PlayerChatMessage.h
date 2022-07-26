// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "PlayerChatMessage.generated.h"

USTRUCT(BlueprintType)
struct FPlayerChatMessage
{
	GENERATED_BODY()

	FPlayerChatMessage() : UserName(""), Message("") {};
	FPlayerChatMessage(FString name, FString message) : UserName(name), Message(message) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message = "";
};
