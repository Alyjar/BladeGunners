#include "HealthPickup.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Health.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();

	UCapsuleComponent* CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnOverlapBegin);

	GetWorld()->GetTimerManager().SetTimer(DespawnTimerHandle, this, &AHealthPickup::DespawnKit, DespawnTime, false, DespawnTime);
}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPickup::DespawnKit()
{
	Destroy();
}

void AHealthPickup::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UHealth* playerHealth;
	playerHealth = OtherActor->FindComponentByClass<UHealth>();

	if (!playerHealth)
		return;

	playerHealth->Heal(HealAmount);

	Destroy();
}

