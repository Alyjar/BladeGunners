// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class SKATING_FPS_API AWeaponShotgun : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeaponShotgun();
	virtual void StartFire() override;
protected:
	virtual void Fire() override;
private:
	FVector2D RandPointInCircle(float radius);
public:

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int NumOfPellets = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float BulletAngle;
};
