// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponADS.generated.h"

/**
 * 
 */
class UCameraComponent;
UCLASS()
class SKATING_FPS_API AWeaponADS : public AWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool IsAds = false;

	virtual void StartFire();
	virtual void StopFire();
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ADS(UCameraComponent* Camera, float FOV, bool Swapping = false);
	UFUNCTION(BlueprintImplementableEvent)
	void OnADS();

protected:
	virtual void Fire() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	float SniperADSFOV = 45;

	void TimerElasped();
};