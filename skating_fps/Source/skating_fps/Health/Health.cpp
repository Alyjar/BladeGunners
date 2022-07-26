#include "Health.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "../Movement/PlayerMovement.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "GameFramework/Actor.h"

UHealth::UHealth()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealth::BeginPlay()
{
	Super::BeginPlay();
	SetHealth(MaxHealth);
	SetShield(MaxShield);
	SetOverShield(100);
	Cast<APlayerMovement>(this->GetOwner())->Health = this;

	if (RespawnShieldMaterialParameter)
	{
		//inst = GetWorld()->GetParameterCollectionInstance(RespawnShieldMaterialParameter);
	}
}

void UHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (true)
	{
		if (CurrentShield > 0)
		{
			paramIncrease = -CurrentShield - 15;
			Cast<APlayerMovement>(this->GetOwner())->ShieldMesh->SetScalarParameterValueOnMaterials("ShieldErode", paramIncrease);
			//inst->SetScalarParameterValue("ShieldErode", paramIncrease);
		}
		else
		{
			paramIncrease = 50.0f;
			Cast<APlayerMovement>(this->GetOwner())->ShieldMesh->SetScalarParameterValueOnMaterials("ShieldErode", paramIncrease);
			//inst->SetScalarParameterValue("ShieldErode", paramIncrease);
		}
	}

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	CurrentShield = FMath::Clamp(CurrentShield, 0.0f, MaxShield);
	CurrentOverShield = FMath::Clamp(CurrentOverShield, 0.0f, MaxOverShield);

	if (ChargingShield)
		RechargeShield();

	if (DrainingOverShield)
		DrainOverShield();

	if (CurrentHealth <= 0)
	{
		StopChargingShield();
		StopOvershieldAudio();
	}
}

void UHealth::SetHealth(float healthValue)
{
	CurrentHealth = healthValue;
}

void UHealth::Heal(float healValue)
{
	SetHealth(CurrentHealth + healValue);

	GainHealthAudio();
}

void UHealth::GainHealthAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::SetShield(float shieldValue)
{
	CurrentShield = shieldValue;
}

void UHealth::SetOverShield(float overShieldValue)
{
	CurrentOverShield = overShieldValue;

	DelayOverShieldDrain();
}

void UHealth::DelayShieldCharge()
{
	GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UHealth::RechargeShield, ShieldChargeDelay, false);
}

void UHealth::DelayOverShieldDrain()
{
	GetWorld()->GetTimerManager().SetTimer(DrainTimerHandle, this, &UHealth::DrainOverShield, OverShieldDrainDelay, false);
}

void UHealth::RechargeShield()
{
	ChargingShield = true;
	CurrentShield += ShieldChargeRate;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);

	ShieldRechargingAudio();

	if (CurrentShield >= MaxShield)
	{
		SetShield(MaxShield);
		ChargingShield = false;
		StopChargingShield();
	}
}

void UHealth::StopChargingShield_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::ShieldRechargingAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::DrainOverShield()
{
	DrainingOverShield = true;
	CurrentOverShield -= OverShieldDrainRate;
	GetWorld()->GetTimerManager().ClearTimer(DrainTimerHandle);

	OvershieldDepletingAudio();

	if (CurrentOverShield <= 0)
	{
		SetOverShield(0);
		DrainingOverShield = false;
		StopOvershieldAudio();
	}
}

void UHealth::OvershieldDepletingAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::StopOvershieldAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::ShieldBrokenAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::TakeDamage(float damageValue)
{
	ChargingShield = false;
	StopChargingShield();

	if (CurrentOverShield > 0) 
	{
		if (CurrentOverShield - damageValue <= 0)
		{
			float remainingDamage = CurrentOverShield - damageValue;
			SetOverShield(0);
			SetShield(CurrentShield + remainingDamage);

			ShieldHitAudio();
		}
		else
			SetOverShield(CurrentOverShield - damageValue);

			ShieldHitAudio();
	}
	else if (CurrentShield > 0)
	{
		if (CurrentShield - damageValue <= 0) 
		{
			float remainingDamage = CurrentShield - damageValue;
			SetShield(0);
			SetHealth(CurrentHealth + remainingDamage);
			
			ShieldBrokenAudio();
			ShieldHitAudio();
			TakeDamageAudio();
		}
		else
			SetShield(CurrentShield - damageValue);

			ShieldHitAudio();
	}
	else
	{
		SetHealth(CurrentHealth - damageValue);

		TakeDamageAudio();
	}

	DelayShieldCharge();
}

void UHealth::TakeDamageAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::ShieldHitAudio_Implementation()
{
	//Audio BP Event Function.
}

void UHealth::DamageDEBUG()
{
	TakeDamage(40);
}

void UHealth::OvershieldDEBUG()
{
	SetOverShield(MaxOverShield);
}

void UHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealth, CurrentHealth);
	DOREPLIFETIME(UHealth, CurrentShield);
	DOREPLIFETIME(UHealth, CurrentOverShield);
}
