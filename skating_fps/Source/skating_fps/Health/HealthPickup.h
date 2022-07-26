#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

UCLASS()
class SKATING_FPS_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickup();

	UPROPERTY(EditAnywhere, Category = "Healing")
		float HealAmount;
	UPROPERTY(EditAnywhere, Category = "Despawn Time")
		float DespawnTime;

	UFUNCTION()
		void DespawnKit();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle DespawnTimerHandle;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
