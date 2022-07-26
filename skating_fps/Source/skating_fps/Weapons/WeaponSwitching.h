// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSwitching.generated.h"


class APlayerMovement;
class AWeaponBase;
UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SKATING_FPS_API UWeaponSwitching : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponSwitching();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TArray<AWeaponBase*> EquipedWeapons;

	UFUNCTION(BlueprintCallable)
		void SwapWeapon(int i);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeapon(int i);

	UFUNCTION(Server, Reliable)
		void Drop(int i);

	//Audio Implementation Here.
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void SwitchWeaponAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void PickupWeaponAudio();

	void DevModeWeaponSpawn();

	void CheckWeaponPickUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWeaponBase* CurrentWeapon;

private:
	UFUNCTION(Server, Reliable)
		void PickUp(AWeaponBase* weapon);
	UFUNCTION(Server, Reliable)
	void EquipWeapon(AWeaponBase* weapon);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AWeaponBase>> Weapons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<AWeaponBase>> StartingWeapons;
	APlayerMovement* Player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		bool DevMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUp", meta = (AllowPrivateAccess = "true"))
		USceneComponent* PickUpSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp", meta = (AllowPrivateAccess = "true"))
		float PickUpRadius = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TEnumAsByte<ECollisionChannel> DetectionChannel = ECC_EngineTraceChannel2;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PickUp", meta = (AllowPrivateAccess = "true"))
		int InventorySlots = 2;
	UPROPERTY(Replicated)
	int CurrentWeaponNum = 0;
};

