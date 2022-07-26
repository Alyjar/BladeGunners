// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "SuicidePistol.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API ASuicidePistol : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	virtual void LineTrace(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection, AActor* MyOwner, FVector TraceEnd) override;
};
