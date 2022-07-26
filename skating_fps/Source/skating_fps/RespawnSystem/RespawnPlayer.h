#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Health/Health.h"
#include "Net/UnrealNetwork.h"
#include "RespawnPlayer.generated.h"

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SKATING_FPS_API URespawnPlayer : public UActorComponent
{
	GENERATED_BODY()

public:
	URespawnPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn Delay")
	float DestroyPawnDelayTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn Delay")
	bool isRespawning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString DissolveKey = "RespawnDissolve";
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	float DissolveValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float DissolveSpeed = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString DissolveColourKey = "DissolveColour";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FLinearColor FadeColour = FColor::Red;

	UPROPERTY(replicated)
	bool isDissapearing;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* PlayerSkeleton = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	USkeletalMesh* CorpseMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	UMaterialInterface* CorpseMaterial;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	UMaterialParameterCollection* CorpseMaterialParameter;

	UMaterialParameterCollectionInstance* inst;

	UFUNCTION(NetMulticast, Reliable)
	void Death();

	UFUNCTION(NetMulticast, Reliable)
	void RespawnDelay();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<AActor> HealthDropObject;

	//Becca's Audio Implementation Header File Area.
	UFUNCTION(BlueprintNativeEvent, Category = "Wwise")
	void DeathAudioTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float RespawnDelayTime;

	UFUNCTION(Server, Reliable)
	void DestroyPawn();

	UFUNCTION()
	void DropHealthKit();

	UHealth* PlayerHealth;

	FTimerHandle RespawnTimerHandle;
	FTimerHandle DestroyPawnTimerHandle;
	FTimerHandle HealthSpawnTimerHandle;

	FTimerDelegate RespawnTimerDel;
};
