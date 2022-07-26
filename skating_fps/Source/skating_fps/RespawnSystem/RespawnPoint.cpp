#include "RespawnPoint.h"
#include "Components/StaticMeshComponent.h"

ARespawnPoint::ARespawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = MeshComp;
	SetActorHiddenInGame(true);
}

void ARespawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARespawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

