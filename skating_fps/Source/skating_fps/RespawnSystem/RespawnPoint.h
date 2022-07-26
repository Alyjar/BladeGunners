#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnPoint.generated.h"

UCLASS()
class SKATING_FPS_API ARespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ARespawnPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "SpawnPoint");
	class UStaticMeshComponent* MeshComp;
};
