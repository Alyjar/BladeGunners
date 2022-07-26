#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ViewableCamera.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class SKATING_FPS_API AViewableCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AViewableCamera();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USceneComponent* ViewLocation = nullptr;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* CameraBase = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* CameraHead = nullptr;
};
