// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponPistol.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API AWeaponPistol : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;

	virtual void StopFire() override;
	
};
