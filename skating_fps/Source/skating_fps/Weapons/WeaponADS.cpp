// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponADS.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "../Movement/PlayerMovement.h"


void AWeaponADS::StartFire()
{
	Super::StartFire();
}

void AWeaponADS::StopFire()
{
	Super::StopFire();
}

void AWeaponADS::Fire()
{
	Super::Fire();
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AWeaponADS::TimerElasped, 0.1f, false, 0.1f);
}

void AWeaponADS::TimerElasped()
{
	if (IsAds)
	{
		ADS(Cast<APlayerMovement>(this->GetOwner())->CameraComponent, 90.0f, false);
	}
}

void AWeaponADS::ADS_Implementation(UCameraComponent* Camera, float FOV, bool Swapping)
{
	
	if (Camera)
	{
		//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("ADS")));
		if (!IsAds && Swapping)
		{
			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("END")));
			return;
		}
		//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("ADS")));
		Camera->SetFieldOfView(FOV);
		OnADS();
	}
}

void AWeaponADS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponADS, IsAds);

}