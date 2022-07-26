// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../PlayerStates/MainPlayerState.h"
#include "../Structs/TeamData.h"

#include "CoreMinimal.h"
#include "TeamPlayerState.generated.h"

UCLASS()
class SKATING_FPS_API ATeamPlayerState : public AMainPlayerState
{
	GENERATED_BODY()

public:
	ATeamPlayerState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_CloseTeamSelectWindow();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FTeamData Team = FTeamData();
};
