#pragma once

class USplineComponent;
class UStaticMesh;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RailSystem.generated.h"

UCLASS()
class SKATING_FPS_API ARailSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	ARailSystem();

	UFUNCTION()
	int GetClosestSplinePoint(FVector location);

	UFUNCTION()
	float GetTimeAtLocation(FVector location, float accuracy = 0.1f);

	UFUNCTION()
	float GetDistanceToNextPoint(int direction, FVector currentLocation);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GrindSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* MeshUsed = nullptr;
};
