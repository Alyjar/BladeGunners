// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Net/UnrealNetwork.h"

#include "../Enums/PlayerTeam.h"

#include "GameFramework/PlayerController.h"
#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "TDMSpectatorPawn.generated.h"

UCLASS()
class SKATING_FPS_API ATDMSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ATDMSpectatorPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
