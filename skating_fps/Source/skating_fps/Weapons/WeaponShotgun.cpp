// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponShotgun.h"

#include "DrawDebugHelpers.h"
#include "Components/TimelineComponent.h"
#include "kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "../Movement/PlayerMovement.h"

AWeaponShotgun::AWeaponShotgun()
{
}

void AWeaponShotgun::StartFire()
{
	Super::StartFire();
}

void AWeaponShotgun::Fire()
{

	if (currentClipAmmo <= 0)
	{
		NoAmmoAudio(); //Audio Placement.
		StopFire();
		return;
	}

	isWeaponCharging = false;
	GetWorld()->GetTimerManager().ClearTimer(WeaponRechargeTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WeaponRailChargeTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(InitialChargeDelay);

	AActor* MyOwner = this->GetOwner();

	if (MyOwner)
	{
		if (MyTimeline && !MyTimeline->IsPlaying())
		{
			MyTimeline->PlayFromStart();
		}

		FVector EyeLocation;
		FRotator EyeRotation;
		FVector ShotDirection;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		for (int i = 0; i < NumOfPellets; i++)
		{			
			ShotDirection = EyeRotation.Vector();

			float Radius = FMath::Tan(FMath::DegreesToRadians(BulletAngle)) * 10000;

			FVector2D Spread = RandPointInCircle(Radius);

			FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

			TraceEnd = TraceEnd + (Spread.X * MyOwner->GetActorRightVector()) + (Spread.Y * MyOwner->GetActorUpVector());

			LineTrace(EyeLocation, EyeRotation, ShotDirection, MyOwner, TraceEnd);
		}
		//GunFireAudio(); //Audio Placement.
		PlayParticle();
	}

	currentClipAmmo--;
	LastFireTime = GetWorld()->TimeSeconds;
}

FVector2D AWeaponShotgun::RandPointInCircle(float radius)
{
	FVector2D Point;
	float Angle = FMath::RandRange(0.0f, 360.0f);

	float DistanceFromCenter = FMath::RandRange(0.0f, radius);

	Point.X = FMath::Cos(FMath::DegreesToRadians(Angle)) * DistanceFromCenter;
	Point.Y = FMath::Sin(FMath::DegreesToRadians(Angle)) * DistanceFromCenter;

	return Point;
}
