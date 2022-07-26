#pragma once

#include "Net/UnrealNetwork.h"

class ARailSystem;
class UCameraComponent;
class ACharacter;
class USceneComponent;
class USplineComponent;

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AkGameplayStatics.h"
#include "GrindingSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SKATING_FPS_API UGrindingSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrindingSystem();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	bool IsAttached();

	//Becca's Audio Implementation Header File Area.
	UPROPERTY(EditAnywhere, Category = "Wwise")
	class UAkAudioEvent* AttachEvent;
	int32 AttachPlayingId;
	void PostAttachEvent();

	UPROPERTY(EditAnywhere, Category = "Wwise")
	class UAkAudioEvent* DetachEvent;
	int32 DetachPlayingId;
	void PostDetachEvent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void AutoIncreaseVelocity(float DeltaTime);
	UFUNCTION()
	void AutoIncreaseTime(float DeltaTime, ARailSystem* rail);

	UFUNCTION()
	void InputIncreaseTime(float DeltaTime, ARailSystem* rail, const FVector input);

	UFUNCTION()
	float GetDirectionOnSpline(ARailSystem* rail);

	UFUNCTION()
	void SetVerticalInput(float value);
	UFUNCTION()
	void SetHorizontalInput(float value);

	UFUNCTION()
	void CheckForSpline();
	
	UFUNCTION()
	void MoveAlongRail(float DeltaTime, ARailSystem* rail);

	UFUNCTION(Server, Reliable)
	void Server_UpdateTime(float time);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_UpdateTime(float time);

	UFUNCTION(Server, Reliable)
	void Server_SetPosition(FVector position);

	UFUNCTION(Server, Reliable)
	void Server_SetAttachedRail(ARailSystem* rail);

	UFUNCTION()
	void CheckForEnd();
	UFUNCTION()
	void LaunchOffRail();

	UFUNCTION()
	FVector GetRailExtents(ARailSystem* rail);

public:
	ACharacter* Character = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float VelocityScale = 0.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector InputDir = FVector::ZeroVector;

	// 0.05f is precise enough for it to be accurate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DetectionAccuracy = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DetectionRadius = 50.0f;

	// 1.0f means they are required to be directly above it.
	// -1.0f means they can go on from below it as well.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float VerticalDetectionLeniency = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector DetectionOffset = FVector::DownVector * 100.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* DetectionTransform = nullptr;

	// Should be Rail.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> DetectionChannel = ECC_EngineTraceChannel2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true", EditCondition = "false"))
	ARailSystem* AttachedRail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector AttachOffset = FVector::UpVector * 35.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimTolerance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float GrindAddAcceleration = 0.1f;

	// 1.0f to directly up, -1.0f for directly down.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float VerticalJumpModifier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float VelocityScaleIncrease = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int LastDirection = 0;

	// From 0.0f-1.0f. Recommended to be 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RailLeaveLeniency = 0.05f;

	// This is in regards for when the player reaches the end of the rail and
	// is automatically kicked off. Otherwise they would keep rejoining it.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanRejoin = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxKickOffReturnVelocity = 0.0f;
};
