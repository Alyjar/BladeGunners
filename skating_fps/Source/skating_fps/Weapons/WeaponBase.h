// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Components/GrindingSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UTimelineComponent;
class UNiagaraSystem;
UCLASS()
class SKATING_FPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartFire();
	virtual void StopFire();
	virtual void RailChargeWeapon();
	virtual void RechargeWeapon();

	//Audio Implementation Here.
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void GunFireAudio(AActor* actor);
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void NoAmmoAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void OnStartWeaponRecharge();
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void OnStopWeaponRecharge();
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void OnMaxAmmo();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		bool playerNotified = true;

	//Temporarily commented until implemented properly.
	UFUNCTION(BlueprintNativeEvent, Category = "Audio")
		void HeadshotAudio();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PickUp")
		void PickedUp();
	void StartWeaponDestroy();
	void StopWeaponDestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerLineTrace(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection, AActor* MyOwner, FVector TraceEnd);

	virtual void Fire();
	virtual void LineTrace(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection, AActor* Owner, FVector TraceEnd);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void PlayParticle();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hit")
		void OnHit(AActor* HitActor, FVector HitLocation, FVector HitNormal, FHitResult HitInfo);

	FVector BulletSpread(FVector Vector);

private:
	UFUNCTION()
	void TimelineCallback(float value);

	UFUNCTION()
	void TimelineFinishedCallback();

	void ResetRecoil();

	UFUNCTION(NetMulticast, Reliable)
	void BulletImpact(AActor* Actor, FVector Location, FVector Normal, UParticleSystem* SelectedEffect);
	UFUNCTION(NetMulticast, Reliable)
	void BulletTracer(FVector Location, FVector BeamStart, FVector BeamEnd);

	UFUNCTION(NetMulticast, Reliable)
	void DestroyWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "PickUp", meta = (AllowPrivateAccess = "true"))
		bool Wielded = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Mesh;

protected:
	// Weapon properties 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float RoundsPerMinute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		int currentClipAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		int currentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float timeBeforeAmmoRecharge = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float ammoRailChargeTimerRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float ammoRechargeTimerRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UDamageType> DamageType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float Damage = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float DamageModifier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UTexture* WeaponIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UTexture* WeaponBarFull;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UTexture* WeaponBarEmpty;

	UPROPERTY(BlueprintReadWrite)
	float TimeBetweenShots;
	float LastFireTime;

	UTimelineComponent* MyTimeline;

	FTimerHandle WeaponRailChargeTimerHandle;
	FTimerHandle WeaponRechargeTimerHandle;
	FTimerHandle TimerHandle_FireRate;
	FTimerHandle InitialChargeDelay;

	bool isWeaponCharging;


private:
	// Weapon Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		int maxClipAmmo = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		int maxAmmo = 90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float MinSpread = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float MaxSpread = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float WeaponDestroyDelay = 5.0f;
	FTimerHandle SpawnHandle;

	//VFX
	//Decal Impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* DecalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		FVector DecalSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float DecalLifeTime;
	//Impact Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* DefaultImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* FleshImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		FVector ImpactParticleSize;
	// Bullet Tracer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UNiagaraSystem* BulletTracerVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float BeamTracerLifeTime;


	//Recoil
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* VerticalRecoil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* HorizontalRecoil;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float DefaultRecoilModifier = 1.0f;
	float RecoilModifier;
	UPROPERTY(VisibleAnywhere)
		FRotator OriginPitchYaw;
	UPROPERTY(VisibleAnywhere)
		FRotator EndPitchYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		float RecoilMaxThreshold;

	//Other
	UPROPERTY(VisibleAnywhere)
		UGrindingSystem* GrindingSystem;
};
