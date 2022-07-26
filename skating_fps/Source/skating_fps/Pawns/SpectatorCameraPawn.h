#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpectatorCameraPawn.generated.h"

class AViewableCamera;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSwapSignature, const FString&, Index);

UCLASS()
class SKATING_FPS_API ASpectatorCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ASpectatorCameraPawn();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_CreateHUD();

	UFUNCTION(BlueprintNativeEvent)
	void CreateHUD();

	UFUNCTION(BlueprintCallable)
	void CycleUp();
	UFUNCTION(BlueprintCallable)
	void CycleDown();

	UFUNCTION(BlueprintCallable)
	void CycleIndex(int direction);

	UFUNCTION(BlueprintCallable)
	void SetPositionAndRotation(int index);

	UFUNCTION(BlueprintCallable)
	void SetRandomPosition();

protected:
	TArray<AViewableCamera*> GetAllCameras();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName CycleDownKey = "SwitchCameraLeft";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName CycleUpKey = "SwitchCameraRight";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<AViewableCamera*> Cameras = TArray<AViewableCamera*>();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int CameraIndex = 0;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, meta = (AllowPrivateAccess = true))
	FOnCameraSwapSignature OnCameraSwap = FOnCameraSwapSignature();
};
