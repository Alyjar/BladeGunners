#include "SpectatorCameraPawn.h"

#include "../Actors/ViewableCamera.h"
#include "../Controllers/FPSController.h"

#include "Kismet/GameplayStatics.h"

ASpectatorCameraPawn::ASpectatorCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpectatorCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASpectatorCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpectatorCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(CycleDownKey, EInputEvent::IE_Pressed, this, &ASpectatorCameraPawn::CycleDown);
	PlayerInputComponent->BindAction(CycleUpKey, EInputEvent::IE_Pressed, this, &ASpectatorCameraPawn::CycleUp);
}

void ASpectatorCameraPawn::CycleUp()
{
	CycleIndex(1);
}

void ASpectatorCameraPawn::CycleDown()
{
	CycleIndex(-1);
}

void ASpectatorCameraPawn::CycleIndex(int direction)
{
	if (direction == 0)
		return;

	CameraIndex += FMath::Abs(direction) / direction;
	
	CameraIndex = CameraIndex >= Cameras.Num() ? 0 : CameraIndex;
	CameraIndex = CameraIndex < 0 ? Cameras.Num() - 1 : CameraIndex;

	SetPositionAndRotation(CameraIndex);
}

void ASpectatorCameraPawn::SetPositionAndRotation(int index)
{
	if (index < 0 || index >= Cameras.Num())
		return;

	AViewableCamera* camera = Cameras[index];
	if (camera->ViewLocation)
	{
		SetActorTransform(camera->ViewLocation->GetComponentTransform());
		OnCameraSwap.Broadcast(FString::FromInt(index));
	}
}

void ASpectatorCameraPawn::SetRandomPosition()
{
	CameraIndex = FMath::RandRange(0, Cameras.Num() - 1);
	SetPositionAndRotation(CameraIndex);
}

TArray<AViewableCamera*> ASpectatorCameraPawn::GetAllCameras()
{
	TArray<AActor*> actors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AViewableCamera::StaticClass(), actors);

	TArray<AViewableCamera*> cameras = TArray<AViewableCamera*>();
	for (AActor* actor : actors)
	{
		AViewableCamera* camera = Cast<AViewableCamera>(actor);
		if (camera)
			cameras.Add(camera);
	}

	return cameras;
}

void ASpectatorCameraPawn::Client_CreateHUD_Implementation()
{
	Cameras = GetAllCameras();

	CreateHUD();
}

void ASpectatorCameraPawn::CreateHUD_Implementation()
{

}
