// Fill out your copyright notice in the Description page of Project Settings.


#include "SuicidePistol.h"
#include "../skating_fps.h"
#include "kismet/GameplayStatics.h"

void ASuicidePistol::StartFire()
{
	Super::StartFire();
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	if (FirstDelay == 0.0f)
	{
		Fire();
	}
}

void ASuicidePistol::StopFire()
{
	Super::StopFire();
}

void ASuicidePistol::LineTrace(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection, AActor* MyOwner, FVector TraceEnd)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerLineTrace(EyeLocation, EyeRotation, ShotDirection, MyOwner, TraceEnd);
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams);
	UGameplayStatics::ApplyPointDamage(this->GetOwner(), Damage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

}