// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSpawner.h"
#include "WeaponBase.h"


// Sets default values
AWeaponSpawner::AWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Actor Mesh"));
	ActorMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponSpawner::BeginPlay()
{
	ActorMesh->SetVisibility(false);
	SpawnWeapon();
}

// Called every frame
void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponSpawner::StartSpawnWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("ran")));
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &AWeaponSpawner::SpawnWeapon, SpawnDelay, false, SpawnDelay);
}


void AWeaponSpawner::SpawnWeapon_Implementation()
{
	if (SpawnableActor.Num() > 1) {
		ActorToSpawn++;
		if (ActorToSpawn >= SpawnableActor.Num())
		{
			ActorToSpawn = 0;
		}
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnableActor[ActorToSpawn], this->GetActorLocation(), FRotator::ZeroRotator, SpawnParameters);
	SpawnedActor->SetOwner(this);
	GetWorldTimerManager().ClearTimer(SpawnHandle);
}

