#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerMovement.generated.h"

class USphereComponent;
class ATeamPlayerState;
class AWeaponBase;
class UCameraComponent;
class UHealth;
class UGrindingSystem;
class UScoreboardComponent;
class UWeaponSwitching;
class WeaponBase;
class USkatingMovementComponent;

UCLASS()
class SKATING_FPS_API APlayerMovement : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerMovement(const class FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ShieldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	UWeaponSwitching* WeaponSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UHealth* Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	bool isShooting = false;

	UFUNCTION(Server, Reliable)
	void SetIsShooting(bool value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UGrindingSystem* Grinding = nullptr;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable, Category = "Movement")
	USkatingMovementComponent* GetSkatingMovementComponent() const;

	virtual FVector GetPawnViewLocation() const override;

	// To my knowledge the CurrentMaxSpeed variable is incorrect an therefore this
	// won't work as you expect. Use GetVelocity().Size() instead.
	UFUNCTION(BlueprintCallable)
	float GetCurrentMaxSpeed();
	UFUNCTION(BlueprintCallable)
	float GetMaxSpeed();
	UFUNCTION(BlueprintCallable)
	float GetMinSpeed();

	UFUNCTION(BlueprintCallable)
	void LerpCameraFOV(float DeltaTime, float value);

	UFUNCTION(BlueprintCallable)
	void SetLastHeadShot(bool value);
	// Calling this resets the headshot!
	UFUNCTION(BlueprintCallable)
	bool GetIfLastHeadshot();

	//Becca's Audio Implementation Header File Area.
	UPROPERTY(EditAnywhere, Category = "Wwise")
	class UAkAudioEvent* RespawnEvent;
	int32 RespawnPlayingId;
	void PostRespawnEvent();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool UseFOVLerp = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FOVLerpSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinFOV = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxFOV = 105.0f;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool LastHitHeadshot = false;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FTimerHandle MaterialTimerHandle = FTimerHandle();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TeamMaterialCheckLoopTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Acceleration = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Deacceleration = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float DefaultSpeed = 1100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MinSpeed = 500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 2000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float HalfPipeJumpVelocityZ = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float HalfPipeTolerance = -10.0f;
	float CurrentMaxSpeed;

	UMaterialParameterCollectionInstance* inst;
	USkeletalMeshComponent* PlayerSkeleton;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	bool IsDissolving = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString DissolveKey = "RespawnDissolve";
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	float DissolveValue = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float DissolveSpeed = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UMaterialParameterCollection* RespawnMaterialParameter;
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UMaterialInterface* NormalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float floorInfluenceForce = 150000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Looking", meta = (AllowPrivateAccess = "true"))
	float Sensitivity = 20.0f;




	UCharacterMovementComponent* CharacterMovement;

	float ForwardDir;
	float RightDir;

	int CurrentWeaponSlot = 0;

	void StartFire();
	void StopFire();

	void PickUp();


	void MoveForward(float value);
	void MoveRight(float value);
	void StartJump();
	void EndJump();
	void Turn(float value);
	void LookUp(float value);
	float GetGroundInfluence(FVector floorNormal);
	UFUNCTION(Server, Reliable)
	void ServerApplyGroundInfluence(float Speed);

	void WeaponSlotOne();
	void WeaponSlotTwo();
	void WeaponSlotThree();
	void WeaponSlotFour();
	void WeaponScroll(float value);

protected:
	UFUNCTION(BlueprintCallable)
	void OnTeamChange(ATeamPlayerState* swapped);

	UFUNCTION(BlueprintCallable, Server, UnReliable)
	void Server_OnPossessColour();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multi_OnPossessColour(ATeamPlayerState* state);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPossessColour(ATeamPlayerState* state);
};
