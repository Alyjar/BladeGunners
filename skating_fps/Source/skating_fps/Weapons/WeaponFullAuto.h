// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponFullAuto.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API AWeaponFullAuto : public AWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

public:
	virtual void BeginPlay() override;
	virtual void StartFire() override;

	virtual void StopFire() override;

};
