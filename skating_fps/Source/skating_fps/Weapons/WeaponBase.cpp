// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Components/TimelineComponent.h"
#include "Components/DecalComponent.h"
#include "kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../Movement/PlayerMovement.h"
#include "../skating_fps.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "skating_fps/GameStates/MainLevelGameState.h"
#include "skating_fps/Movement/PlayerMovement.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	currentClipAmmo = maxClipAmmo;
	currentAmmo = maxAmmo;
	bReplicates = true;
}

void AWeaponBase::PickedUp_Implementation()
{
}

void AWeaponBase::StartWeaponDestroy()
{
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &AWeaponBase::DestroyWeapon, WeaponDestroyDelay, false, WeaponDestroyDelay);
}

void AWeaponBase::StopWeaponDestroy()
{
	if (SpawnHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(SpawnHandle);
	}
}

void AWeaponBase::DestroyWeapon_Implementation()
{
	this->Destroy();
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetActorScale3D(FVector::OneVector);
	FOnTimelineFloat RecoilCallback;

	FOnTimelineEventStatic onTimelineFinishedCallback;

	/*AActor* MyOwner = this->GetOwner();
	if (MyOwner)
		GrindingSystem = MyOwner->FindComponentByClass<UGrindingSystem>();*/

	if (VerticalRecoil && HorizontalRecoil)
	{
		RecoilCallback.BindUFunction(this, "TimelineCallback");

		onTimelineFinishedCallback.BindUFunction(this, "TimelineFinishedCallback");

		MyTimeline->AddInterpFloat(VerticalRecoil, RecoilCallback, FName("Aplha"));

		MyTimeline->SetTimelineLength(5.0f);
		MyTimeline->SetLooping(false);
		MyTimeline->SetIgnoreTimeDilation(true);
	}
	TimeBetweenShots = 60 / RoundsPerMinute;
}

void AWeaponBase::ServerFire_Implementation()
{
	Fire();
}

bool AWeaponBase::ServerFire_Validate()
{
	return true;
}


void AWeaponBase::ServerLineTrace_Implementation(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection,
	AActor* MyOwner, FVector TraceEnd)
{
	LineTrace(EyeLocation, EyeRotation, ShotDirection, MyOwner, TraceEnd);
}

bool AWeaponBase::ServerLineTrace_Validate(FVector EyeLocation, FRotator EyeRotation, FVector ShotDirection,
	AActor* MyOwner, FVector TraceEnd)
{
	return true;
}

void AWeaponBase::Fire()
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

		APlayerCameraManager* CameraManager = Cast<APlayerController>(Cast<APawn>(MyOwner)->GetController())->PlayerCameraManager;

		EyeLocation = CameraManager->GetCameraLocation();
		EyeRotation = CameraManager->GetCameraRotation();

		ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (BulletSpread(ShotDirection) * 10000);
		LineTrace(EyeLocation, EyeRotation, ShotDirection, MyOwner, TraceEnd);
		 //Audio Placement.
		PlayParticle();
	}

	currentClipAmmo--;
	LastFireTime = GetWorld()->TimeSeconds;
}

void AWeaponBase::LineTrace(FVector EyeLocation, FRotator EyeRotation,FVector ShotDirection, AActor* MyOwner, FVector TraceEnd)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerLineTrace(EyeLocation, EyeRotation, ShotDirection, MyOwner, TraceEnd);
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s"), *TraceEnd.ToString()));

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s"), *HitActor->GetName()));


			BulletTracer(this->GetRootComponent()->GetSocketLocation("Muzzle"), this->GetRootComponent()->GetSocketLocation("Muzzle"), Hit.Location);
			OnHit(HitActor, Hit.Location, Hit.ImpactNormal, Hit);
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			APlayerMovement* player = Cast<APlayerMovement>(HitActor);
			if (player)
				player->SetLastHeadShot(false);

			float ActualDamage = Damage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= DamageModifier;
				HeadshotAudio();

				if (player)
					player->SetLastHeadShot(true);
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("min spread = %f, max spread = %f, Damage = %f"), MinSpread, MaxSpread, ActualDamage));

			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SurfaceType1:
				SelectedEffect = DefaultImpactParticle;
				break;
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactParticle;
				break;
			default:
				SelectedEffect = DefaultImpactParticle;
				break;
			}
			BulletImpact(HitActor, Hit.Location, Hit.ImpactNormal, SelectedEffect);
		}

	}

	//DrawDebugLine(GetWorld(), FVector(EyeLocation.X + .1f, EyeLocation.Y, EyeLocation.Z), TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
}

void AWeaponBase::HeadshotAudio_Implementation()
{

}

void AWeaponBase::PlayParticle_Implementation()
{

}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentClipAmmo = FMath::Clamp(currentClipAmmo, 0, maxClipAmmo);
	currentAmmo = FMath::Clamp(currentAmmo, 0, maxAmmo);

	AActor* MyOwner = this->GetOwner();

	if (MyOwner)
		GrindingSystem = MyOwner->FindComponentByClass<UGrindingSystem>();

	if (GrindingSystem)
	{
		if (GrindingSystem->IsAttached())
		{
			if (!WeaponRailChargeTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(WeaponRailChargeTimerHandle, this, &AWeaponBase::RailChargeWeapon, ammoRailChargeTimerRate, true);

				if (currentAmmo < maxAmmo)
					OnStartWeaponRecharge();
				else if (currentClipAmmo < maxClipAmmo)
					OnStartWeaponRecharge();
			}
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(WeaponRailChargeTimerHandle);
			OnStopWeaponRecharge();
		}
	}
}

void AWeaponBase::OnStartWeaponRecharge_Implementation()
{
	//Audio BP Event Here.
}

void AWeaponBase::OnStopWeaponRecharge_Implementation()
{
	//Audio BP Event Here.
}

void AWeaponBase::OnMaxAmmo_Implementation()
{
	//Audio BP Event Here.
}

void AWeaponBase::RailChargeWeapon()
{
	APlayerMovement* player = Cast<APlayerMovement>(this->GetOwner());
	if (player != nullptr)
	{
		if (player->isShooting == true)
		{
			GetWorld()->GetTimerManager().ClearTimer(WeaponRailChargeTimerHandle);
			OnStopWeaponRecharge();
			if (playerNotified == false)
			{
				playerNotified = true;
			}
			return;
		}
	}
	if (currentClipAmmo < maxClipAmmo)
	{
		currentClipAmmo++;
		playerNotified = false;
	}
	else if (currentAmmo < maxAmmo)
	{
		currentAmmo++;
		playerNotified = false;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponRailChargeTimerHandle);
		OnStopWeaponRecharge();
		if (playerNotified == false)
		{
			OnMaxAmmo();
			playerNotified = true;
		}
	}
}

void AWeaponBase::RechargeWeapon()
{
	if (currentClipAmmo >= maxClipAmmo)
	{ 
		GetWorld()->GetTimerManager().ClearTimer(WeaponRechargeTimerHandle);
		isWeaponCharging = false;
		return;
	}

	currentClipAmmo ++;
	currentAmmo --;

	GetWorld()->GetTimerManager().SetTimer(WeaponRechargeTimerHandle, this, &AWeaponBase::RechargeWeapon, ammoRechargeTimerRate, true);
}

void AWeaponBase::StartFire()
{
	if (currentClipAmmo <= 0)
	{
		return;
	}
	Cast<APlayerMovement>(this->GetOwner())->SetIsShooting(true);

	if (MyTimeline)
	{
		RecoilModifier = DefaultRecoilModifier;
		MyTimeline->Stop();
	}
	OriginPitchYaw = Cast<APawn>(this->GetOwner())->GetControlRotation();

	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_FireRate, this, &AWeaponBase::Fire, TimeBetweenShots, true, FirstDelay);
}

void AWeaponBase::StopFire()
{
	//Audio Parameter needs to go here in the future. This is for stopping a loop.

	// COMMENT THIS OUT TO GET RELOADING SYSTEM

	//currentClipAmmo = maxClipAmmo;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("stopped")));
	Cast<APlayerMovement>(this->GetOwner())->SetIsShooting(false);

	ResetRecoil();

	if (MyTimeline)
	{
		MyTimeline->Stop();
		MyTimeline->Reverse();
	}

	if (currentClipAmmo <= maxClipAmmo && currentAmmo > 0 && !isWeaponCharging)
	{
		GetWorld()->GetTimerManager().SetTimer(InitialChargeDelay, this, &AWeaponBase::RechargeWeapon, timeBeforeAmmoRecharge, true);
		isWeaponCharging = true;
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_FireRate);

}

void AWeaponBase::GunFireAudio_Implementation(AActor* actor)
{
	//Audio Blueprint Implementation Here.
}

void AWeaponBase::NoAmmoAudio_Implementation() 
{
	//Audio Blueprint Implementation Here.
}

void AWeaponBase::TimelineCallback(float value)
{
	float TempRecoilModifier = RecoilModifier * -1.0f;
	float PlaybackPos =  MyTimeline->GetPlaybackPosition();
	APawn* MyOwner = Cast<APawn>(this->GetOwner());

	if (MyTimeline->IsReversing())
	{
		TempRecoilModifier *= -1.0f;
	}

	if (MyOwner)
	{
		MyOwner->AddControllerPitchInput((VerticalRecoil->GetFloatValue(PlaybackPos)* TempRecoilModifier) * GetWorld()->DeltaTimeSeconds);
		MyOwner->AddControllerYawInput((HorizontalRecoil->GetFloatValue(PlaybackPos) * TempRecoilModifier) * GetWorld()->DeltaTimeSeconds);
	}

}

void AWeaponBase::TimelineFinishedCallback()
{

}

void AWeaponBase::ResetRecoil()
{
	EndPitchYaw = Cast<APawn>(this->GetOwner())->GetControlRotation();

	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(EndPitchYaw, OriginPitchYaw);

	float Pitch = DeltaRotator.Pitch;
	Pitch = abs(Pitch);

	if (Pitch > RecoilMaxThreshold)
	{
		Pitch = UKismetMathLibrary::MapRangeClamped(Pitch, RecoilMaxThreshold, 30.0f, 1.0f, 0.0f);
	}
	if (Pitch < RecoilMaxThreshold)
	{
		Pitch = UKismetMathLibrary::MapRangeClamped(Pitch, 0.0f, RecoilMaxThreshold, 0.0f, 1.0f);
	}
	RecoilModifier *= Pitch;
}

void AWeaponBase::OnHit_Implementation(AActor* HitActor, FVector HitLocation, FVector HitNormal, FHitResult HitInfo)
{
}

FVector AWeaponBase::BulletSpread(FVector Vector)
{
	float x = Vector.X;
	float y = Vector.Y;
	float z = Vector.Z;

	x += FMath::FRandRange(MinSpread, MaxSpread);
	y += FMath::FRandRange(MinSpread, MaxSpread);
	z += FMath::FRandRange(MinSpread, MaxSpread);


	return FVector(x, y, z);

}

void AWeaponBase::BulletImpact_Implementation(AActor* Actor, FVector Location, FVector Normal, UParticleSystem* SelectedEffect)
{
	if (Actor != nullptr)
	{
		USceneComponent* AttachPoint = Actor->GetRootComponent();
		FRotator Rotation = Normal.Rotation();

		if (DecalMaterial != nullptr)
		{
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAttached(DecalMaterial, DecalSize, AttachPoint, "None", Location, Rotation, EAttachLocation::KeepWorldPosition, DecalLifeTime);
			Decal->SetFadeScreenSize(0.001f);
		}
		if (SelectedEffect != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(SelectedEffect, AttachPoint, "None", Location, Rotation, ImpactParticleSize, EAttachLocation::KeepWorldPosition, true, EPSCPoolMethod::None, true);
		}
	}
}

void AWeaponBase::BulletTracer_Implementation(FVector Location, FVector BeamStart, FVector BeamEnd)
{
	if (BulletTracerVFX)
	{
		GunFireAudio(this->GetOwner());

		UNiagaraComponent* Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTracerVFX, Location, FRotator::ZeroRotator, FVector::OneVector, false, false, ENCPoolMethod::None, true);
		if (Tracer)
		{
			Tracer->SetNiagaraVariableVec3("BeamStart", BeamStart);
			Tracer->SetNiagaraVariableVec3("BeamEnd", BeamEnd);
			Tracer->SetNiagaraVariableFloat("LifeTime", TimeBetweenShots);
			Tracer->Activate();
		}
	}
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, MinSpread);
	DOREPLIFETIME(AWeaponBase, MaxSpread);
	DOREPLIFETIME(AWeaponBase, Wielded);
}
