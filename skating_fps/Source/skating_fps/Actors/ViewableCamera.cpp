#include "ViewableCamera.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

AViewableCamera::AViewableCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera Base"));
	CameraBase->SetupAttachment(GetRootComponent());

	CameraHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera Head"));
	CameraHead->SetupAttachment(CameraBase);

	ViewLocation = CreateDefaultSubobject<USceneComponent>(TEXT("View Location"));
	ViewLocation->SetupAttachment(CameraHead);
}

void AViewableCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void AViewableCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

