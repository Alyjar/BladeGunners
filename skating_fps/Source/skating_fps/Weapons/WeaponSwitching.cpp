// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSwitching.h"

#include "DrawDebugHelpers.h"
#include "WeaponBase.h"
#include "WeaponADS.h"
#include "WeaponSpawner.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "skating_fps/Movement/PlayerMovement.h"

// Sets default values for this component's properties
UWeaponSwitching::UWeaponSwitching()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	PickUpSphere = CreateDefaultSubobject<USceneComponent>(TEXT("PickUpRadius"));
	SetIsReplicatedByDefault(true);
	DrawDebugSphere(GetWorld(), FVector::ZeroVector, PickUpRadius, 50, FColor::Cyan, true);

}


// Called when the game starts
void UWeaponSwitching::BeginPlay()
{
	AActor* MyOwner = this->GetOwner();
	Player = Cast<APlayerMovement>(MyOwner);
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (DevMode)
		{
			DevModeWeaponSpawn();
			return;
		}
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Player)
		{
			if (StartingWeapons.Num() == 0)
			{
				return;
			}

			for (int i = 0; i < StartingWeapons.Num(); i++)
			{
				FVector HandSocket = Player->HandMesh->GetSocketLocation("weapon_PointSocket");

				EquipedWeapons.Add(GetWorld()->SpawnActor<AWeaponBase>(StartingWeapons[i], HandSocket, FRotator::ZeroRotator, SpawnParameters));

				if (EquipedWeapons[i])
				{
					EquipWeapon(EquipedWeapons[i]);
				}
			}
			if (EquipedWeapons[0])
			{
				CurrentWeapon = EquipedWeapons[0];
			}

			if (CurrentWeapon)
			{
				Player->CurrentWeapon = CurrentWeapon;
				CurrentWeapon->SetActorHiddenInGame(false);
			}
		}
		// Spawn Pistol and AR
	}
}



// Called every frame
void UWeaponSwitching::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponSwitching::SwapWeapon(int i)
{
	if (i > EquipedWeapons.Num() - 1)
	{
		return;
	}

	if (GetOwnerRole() != ROLE_Authority)
	{
		ServerSwapWeapon(i);
	}

	if (CurrentWeapon)
	{
		if (Cast<AWeaponADS>(CurrentWeapon))
		{
			Cast<AWeaponADS>(CurrentWeapon)->ADS(Player->CameraComponent, 90.0f, true);
		}
		CurrentWeapon->SetActorHiddenInGame(true);
	}
	if (i < EquipedWeapons.Num())
	{
		if (EquipedWeapons[i])
		{
			if (GetOwnerRole() != ROLE_Authority)
			{
				Player->CurrentWeapon->StopFire();
				if (CurrentWeapon != EquipedWeapons[i])
					SwitchWeaponAudio();
			}

			
			CurrentWeapon = EquipedWeapons[i];
			Player->CurrentWeapon = CurrentWeapon;
			Player->CurrentWeapon->SetActorHiddenInGame(false);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("weapon failed to exist")));
		}
	}
}

void UWeaponSwitching::DevModeWeaponSpawn()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (Player)
	{

		for (int i = 0; i < Weapons.Num(); i++)
		{
			FVector HandSocket = Player->HandMesh->GetSocketLocation("weapon_PointSocket");

			EquipedWeapons.Add(GetWorld()->SpawnActor<AWeaponBase>(Weapons[i], HandSocket, FRotator::ZeroRotator, SpawnParameters));
			if (EquipedWeapons[i])
			{
				EquipWeapon(EquipedWeapons[i]);
			}
		}
		if (EquipedWeapons[0])
		{
			CurrentWeapon = EquipedWeapons[0];
		}

		if (CurrentWeapon)
		{
			Player->CurrentWeapon = CurrentWeapon;
			CurrentWeapon->SetActorHiddenInGame(false);
		}
	}
}

void UWeaponSwitching::CheckWeaponPickUp()
{
	// PICK UP
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("PickUp")));
	FVector ownerLocation = Player->GetActorLocation();
	TArray<FHitResult> hits = TArray<FHitResult>();
	FCollisionShape collisionSphere = FCollisionShape::MakeSphere(PickUpRadius);

	GetWorld()->SweepMultiByChannel(hits, ownerLocation, ownerLocation, FQuat::Identity, DetectionChannel, collisionSphere);

	AWeaponBase* weaponPickUp = nullptr;
	for (FHitResult& hit : hits)
	{
		AWeaponBase* weapon = Cast<AWeaponBase>(hit.Actor);
		if (weapon)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("PickedUp")));

			if (!weapon->Wielded)
			{
				weaponPickUp = weapon;
				break;
			}
		}
	}
	if (weaponPickUp)
	{
		PickUp(weaponPickUp);
		PickupWeaponAudio();
	}
}

void UWeaponSwitching::EquipWeapon_Implementation(AWeaponBase* weapon)
{
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

	weapon->Wielded = true;
	weapon->SetOwner(Player);
	weapon->AttachToComponent(Player->HandMesh, AttachmentTransformRules, "weapon_PointSocket");
	weapon->SetActorHiddenInGame(true);
}


void UWeaponSwitching::Drop_Implementation(int i)
{
	if (EquipedWeapons[i])
	{
		EquipedWeapons[i]->StopFire();
		EquipedWeapons[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquipedWeapons[i]->Wielded = false;
		EquipedWeapons[i]->SetOwner(nullptr);
		EquipedWeapons[i]->Mesh->SetSimulatePhysics(true);
		EquipedWeapons[i]->Mesh->SetEnableGravity(true);
		EquipedWeapons[i]->StartWeaponDestroy();
		EquipedWeapons[i] = nullptr;
	}
}

void UWeaponSwitching::PickUp_Implementation(AWeaponBase* weapon)
{
	weapon->Wielded = true;

	if (EquipedWeapons[CurrentWeaponNum] != nullptr)
	{
		Drop(CurrentWeaponNum);
	}
	weapon->Mesh->SetSimulatePhysics(false);
	weapon->Mesh->SetEnableGravity(false);
	weapon->StopWeaponDestroy();

	EquipedWeapons[CurrentWeaponNum] = weapon;

	AWeaponSpawner* spawner = Cast<AWeaponSpawner>(weapon->GetOwner());
	if (spawner)
	{
		spawner->StartSpawnWeapon();
	}

	weapon->SetOwner(Player);
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	weapon->AttachToComponent(Player->HandMesh, AttachmentTransformRules, "weapon_PointSocket");
	CurrentWeapon = weapon;

	Player->CurrentWeapon = CurrentWeapon;
}

//Audio Implementation Here.
void UWeaponSwitching::SwitchWeaponAudio_Implementation()
{
	//Audio BP Event Function.
}

void UWeaponSwitching::PickupWeaponAudio_Implementation()
{
	//Audio BP Event Function.
}

void UWeaponSwitching::ServerSwapWeapon_Implementation(int i)
{
	CurrentWeaponNum = i;
	SwapWeapon(i);
}

bool UWeaponSwitching::ServerSwapWeapon_Validate(int i)
{
	return true;
}

void UWeaponSwitching::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponSwitching, EquipedWeapons);
	DOREPLIFETIME(UWeaponSwitching, CurrentWeaponNum);

}

