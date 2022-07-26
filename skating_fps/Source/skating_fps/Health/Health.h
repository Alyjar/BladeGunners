#pragma once

class AActor;
#include "../GameModes/MainLevelGameMode.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Health.generated.h"


UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SKATING_FPS_API UHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UHealth();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, Category = "Health")
	float HealthHealRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Shield")
	float CurrentShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Shield")
	float CurrentOverShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float MaxShield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float MaxOverShield;
	UPROPERTY(EditAnywhere, Category = "Shield")
	float ShieldChargeRate;
	UPROPERTY(EditAnywhere, Category = "Shield")
	float ShieldChargeDelay;
	UPROPERTY(EditAnywhere, Category = "Shield")
	float OverShieldDrainDelay;
	UPROPERTY(EditAnywhere, Category = "Shield")
	float OverShieldDrainRate;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UMaterialParameterCollection* RespawnShieldMaterialParameter;

	bool ChargingShield;
	bool DrainingOverShield;

	void Heal(float value);
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float value);

	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<AActor> HealthDropObject;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Audio Content. Don't worry about it.
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void GainHealthAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void TakeDamageAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void ShieldBrokenAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void ShieldRechargingAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void OvershieldDepletingAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void ShieldHitAudio();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void StopChargingShield();
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
		void StopOvershieldAudio();

private:
	void SetHealth(float value);
	void SetShield(float value);
	void SetOverShield(float value);
	void DelayShieldCharge();
	void DelayOverShieldDrain();
	void RechargeShield();
	void DrainOverShield();

	FTimerHandle ChargeTimerHandle;
	FTimerHandle DrainTimerHandle;

	UMaterialParameterCollectionInstance* inst;
	UPROPERTY(replicated)
		float paramIncrease;

	UFUNCTION(CallInEditor, Category = "Functions", meta = (AllowPrivateAccess = "true"))
	void DamageDEBUG();

	UFUNCTION(CallInEditor, Category = "Functions", meta = (AllowPrivateAccess = "true"))
	void OvershieldDEBUG();

protected:
	virtual void BeginPlay() override;
};
