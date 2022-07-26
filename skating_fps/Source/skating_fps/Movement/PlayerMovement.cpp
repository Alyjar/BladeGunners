#include "PlayerMovement.h"
#include "SkatingMovementComponent.h"
#include "../GameModes/MainLevelGameMode.h"
#include "../Playerstates/TeamPlayerState.h"
#include "../Components/GrindingSystem.h"
#include "../Components/ScoreboardComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "skating_fps/Health/Health.h"
#include "skating_fps/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "skating_fps/Weapons/WeaponADS.h"
#include "skating_fps/Weapons/WeaponSwitching.h"


// Sets default values
APlayerMovement::APlayerMovement(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USkatingMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand Mesh"));
	HandMesh->SetupAttachment(CameraComponent);
	ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shield Mesh"));
	ShieldMesh->SetupAttachment(GetMesh());
	Grinding = CreateDefaultSubobject<UGrindingSystem>(TEXT("Grinding System"));
}

// Called when the game starts or when spawned
void APlayerMovement::BeginPlay()
{
	Super::BeginPlay();
	PlayerSkeleton = GetMesh();
	PostRespawnEvent(); //Audio Here.
	if (RespawnMaterialParameter)
		inst = GetWorld()->GetParameterCollectionInstance(RespawnMaterialParameter);

	IsDissolving = true;

	WeaponSwitching = this->FindComponentByClass<UWeaponSwitching>();
	if (!WeaponSwitching)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, FString::Printf(TEXT("No Weapon Switching Found")));
	}
	CharacterMovement = GetSkatingMovementComponent();
	if (CharacterMovement)
		CharacterMovement->MaxWalkSpeed = DefaultSpeed;

	GetWorld()->GetTimerManager().SetTimer(MaterialTimerHandle, this, &APlayerMovement::Server_OnPossessColour,
										   TeamMaterialCheckLoopTime, true);

	AMainLevelGameMode* gm = GetWorld()->GetAuthGameMode<AMainLevelGameMode>();
	if (gm)
		gm->OnTeamChangeDelegate.AddDynamic(this, &APlayerMovement::OnTeamChange);

	AMainPlayerState* state = GetPlayerState<AMainPlayerState>();
	if (state)
		state->IsAlive = true;
}

// Called every frame
void APlayerMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Purpose of the Bool is to only run this once.
	if (IsDissolving && PlayerSkeleton)
	{
		DissolveValue += DeltaTime * DissolveSpeed;
		PlayerSkeleton->SetScalarParameterValueOnMaterials(FName(*DissolveKey), DissolveValue);

		// The colour of the Fade is done through BP.
	}
	
	else if (!IsDissolving)
		IsDissolving = false;

	if (CharacterMovement)
	{
		FVector FloorNormal = CharacterMovement->CurrentFloor.HitResult.Normal;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%f"), strength));

		if (CharacterMovement->CurrentFloor.HitResult.GetActor())
		{
			if (CharacterMovement->CurrentFloor.HitResult.GetActor()->ActorHasTag("Ramp"))
			{
				if (GetGroundInfluence(FloorNormal) < HalfPipeTolerance)
				{
					//float MovementHalfPipeJumpZ = HalfPipeJumpVelocityZ * this->CurrentMaxSpeed;
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%f"), GetGroundInfluence(FloorNormal)));

					CharacterMovement->JumpZVelocity = HalfPipeJumpVelocityZ;
					Jump();
				}
			}
			else
			{
				CharacterMovement->JumpZVelocity = 500.0f;
			}
		}

		CurrentMaxSpeed = CharacterMovement->MaxWalkSpeed + GetGroundInfluence(FloorNormal);
		CurrentMaxSpeed = FMath::Clamp(CurrentMaxSpeed, MinSpeed, MaxSpeed);
		if (GetGroundInfluence(FloorNormal) == 0.0f)
		{
			//CurrentMaxSpeed = DefaultSpeed;
			CurrentMaxSpeed = FMath::Lerp(CurrentMaxSpeed, DefaultSpeed, Acceleration * GetWorld()->DeltaTimeSeconds);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, FString::Printf(TEXT("%f"), CurrentMaxSpeed));

		CharacterMovement->MaxWalkSpeed = CurrentMaxSpeed;
		ServerApplyGroundInfluence(CurrentMaxSpeed);
	}
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("%s"), *CharacterMovement->Velocity.ToString()));

	if (Grinding && UseFOVLerp)
	{
		if (Cast<AWeaponADS>(CurrentWeapon))
		{
			if (Cast<AWeaponADS>(CurrentWeapon)->IsAds)
			{
				return;
			}
		}

		float t = 0.0f;
		t = (Grinding->IsAttached()) ? Grinding->VelocityScale : GetVelocity().Size() / GetMaxSpeed();
		LerpCameraFOV(DeltaTime, t);
	}
}

// Called to bind functionality to input
void APlayerMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerMovement::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerMovement::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerMovement::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerMovement::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerMovement::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerMovement::EndJump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerMovement::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerMovement::StopFire);

	PlayerInputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &APlayerMovement::WeaponSlotOne);
	PlayerInputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &APlayerMovement::WeaponSlotTwo);
	PlayerInputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &APlayerMovement::WeaponSlotThree);
	PlayerInputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &APlayerMovement::WeaponSlotFour);
	PlayerInputComponent->BindAxis("WeaponScroll", this, &APlayerMovement::WeaponScroll);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &APlayerMovement::PickUp);
}

USkatingMovementComponent* APlayerMovement::GetSkatingMovementComponent() const
{
	return static_cast<USkatingMovementComponent*>(GetCharacterMovement());
}

FVector APlayerMovement::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

float APlayerMovement::GetCurrentMaxSpeed()
{
	return CurrentMaxSpeed;
}

float APlayerMovement::GetMaxSpeed()
{
	return MaxSpeed;
}

float APlayerMovement::GetMinSpeed()
{
	return MinSpeed;
}

void APlayerMovement::LerpCameraFOV(float DeltaTime, float value)
{
	if (!CameraComponent)
		return;

	float TargetFOV = FMath::Lerp(MinFOV, MaxFOV, value);
	float FOV = FMath::Lerp(CameraComponent->FieldOfView, TargetFOV, DeltaTime * FOVLerpSpeed);

	CameraComponent->SetFieldOfView(FOV);
}

void APlayerMovement::SetLastHeadShot(bool value)
{
	LastHitHeadshot = value;
}

bool APlayerMovement::GetIfLastHeadshot()
{
	bool _headshot = LastHitHeadshot;
	LastHitHeadshot = false;

	return _headshot;
}

void APlayerMovement::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void APlayerMovement::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void APlayerMovement::PickUp()
{
	if (WeaponSwitching)
	{
		WeaponSwitching->CheckWeaponPickUp();
	}
}

void APlayerMovement::MoveForward(float value)
{
	if (Grinding && Grinding->IsAttached())
		return;
	
	FVector Direction = FRotationMatrix(this->GetActorForwardVector().Rotation()).GetScaledAxis(EAxis::X);
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%s"), *CharacterMovement->Velocity.ToString()));


	if (value != 0.0f)
	{
		ForwardDir = FMath::Lerp(ForwardDir, value, Acceleration * GetWorld()->DeltaTimeSeconds);
	}
	if (value == 0.0f)
	{
		ForwardDir = FMath::Lerp(ForwardDir, value, Deacceleration * GetWorld()->DeltaTimeSeconds);
	}

	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Forward Value: %f"), value));

	AddMovementInput(Direction, ForwardDir);
}

void APlayerMovement::MoveRight(float value)
{
	if (Grinding && Grinding->IsAttached())
		return;

	FVector Direction = FRotationMatrix(this->GetActorRightVector().Rotation()).GetScaledAxis(EAxis::X);
	if (value != 0.0f)
	{
		RightDir = FMath::Lerp(RightDir, value, Acceleration * GetWorld()->DeltaTimeSeconds);
	}
	if (value == 0.0f)
	{
		RightDir = FMath::Lerp(RightDir, value, Deacceleration * GetWorld()->DeltaTimeSeconds);
	}
	AddMovementInput(Direction, RightDir);
}

void APlayerMovement::StartJump()
{
	Jump();
}

void APlayerMovement::EndJump()
{
	StopJumping();
}

void APlayerMovement::Turn(float value)
{
	AddControllerYawInput((value * Sensitivity));
}

void APlayerMovement::LookUp(float value)
{
	AddControllerPitchInput((value * Sensitivity));
}

float APlayerMovement::GetGroundInfluence(FVector floorNormal)
{

	if (floorNormal == FVector::UpVector)
	{
		return 0.f;
	}
	FVector Direction = FVector::CrossProduct(floorNormal, FVector::CrossProduct(floorNormal, FVector::UpVector));
	Direction.Normalize();
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%s"), *Direction.ToString()));
	float strength = 1.0f - FVector::DotProduct(floorNormal, FVector::UpVector);
	strength = FMath::Clamp(strength, 0.0f, 1.0f);
	strength *= floorInfluenceForce;

	if (this->GetController() == nullptr)
	{
		return 0.f;
	}
	FVector PlayerDirection = FRotationMatrix(this->GetController()->GetControlRotation()).GetScaledAxis(EAxis::X);
	PlayerDirection.Normalize();
	float dotPlayer = FVector::DotProduct(PlayerDirection, Direction);

	Direction *= strength;



	return strength * dotPlayer;
}

void APlayerMovement::ServerApplyGroundInfluence_Implementation(float Speed)
{
	CharacterMovement->MaxWalkSpeed = Speed;
	//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%f"), CharacterMovement->MaxWalkSpeed));
}

#pragma region WeaponSwapping

void APlayerMovement::SetIsShooting_Implementation(bool value)
{
	isShooting = value;
}


void APlayerMovement::WeaponSlotOne()
{
	CurrentWeaponSlot = 0;
	if (WeaponSwitching)
	{
		WeaponSwitching->SwapWeapon(CurrentWeaponSlot);
	}
}

void APlayerMovement::WeaponSlotTwo()
{
	CurrentWeaponSlot = 1;

	if (WeaponSwitching)
	{
		WeaponSwitching->SwapWeapon(CurrentWeaponSlot);
	}
}

void APlayerMovement::WeaponSlotThree()
{
	CurrentWeaponSlot = 2;

	if (WeaponSwitching)
	{
		WeaponSwitching->SwapWeapon(CurrentWeaponSlot);
	}
}

void APlayerMovement::WeaponSlotFour()
{
	CurrentWeaponSlot = 3;

	if (WeaponSwitching)
	{
		WeaponSwitching->SwapWeapon(CurrentWeaponSlot);
	}
}

void APlayerMovement::WeaponScroll(float value)
{
	if (value == 0)
	{
		return;
	}
	if (WeaponSwitching->EquipedWeapons.Num() == 0)
	{
		return;
	}
	CurrentWeaponSlot += value;
	if (CurrentWeaponSlot < 0)
	{
		CurrentWeaponSlot = WeaponSwitching->EquipedWeapons.Num() - 1;
	}
	if (CurrentWeaponSlot > WeaponSwitching->EquipedWeapons.Num() - 1)
	{
		CurrentWeaponSlot = 0;
	}

	WeaponSwitching->SwapWeapon(CurrentWeaponSlot);
}

#pragma endregion

// Don't know why this isn't called.
// It gets binded and broadcasted, but nothing.
// Fuck it.
void APlayerMovement::OnTeamChange(ATeamPlayerState* swapped)
{
	ATeamPlayerState* current = GetPlayerState<ATeamPlayerState>();
	if (current == swapped)
		Multi_OnPossessColour(swapped);
}

void APlayerMovement::Server_OnPossessColour_Implementation()
{
	Multi_OnPossessColour(GetPlayerState<ATeamPlayerState>());
}

void APlayerMovement::Multi_OnPossessColour_Implementation(ATeamPlayerState* state)
{
	OnPossessColour(state);
}

void APlayerMovement::OnPossessColour_Implementation(ATeamPlayerState* state)
{
}

void APlayerMovement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerMovement, CurrentWeapon);
	DOREPLIFETIME(APlayerMovement, isShooting);
	DOREPLIFETIME(APlayerMovement, LastHitHeadshot);
}

void APlayerMovement::PostRespawnEvent()
{
	//Audio Function.
	FOnAkPostEventCallback nullCallback;
	RespawnPlayingId = UAkGameplayStatics::PostEvent(RespawnEvent, this->GetOwner(), int32(0), nullCallback, TArray<FAkExternalSourceInfo>());
}