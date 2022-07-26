#include "RespawnPlayer.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Camera/CameraComponent.h"
#include "../FunctionLibraries/General.h"
#include "../PlayerStates/MainPlayerState.h"
#include "../Weapons/WeaponSwitching.h"
#include "../Weapons/WeaponBase.h"
#include "../Movement/SkatingMovementComponent.h"
#include "../Controllers/FPSController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "../Movement/PlayerMovement.h"

URespawnPlayer::URespawnPlayer()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URespawnPlayer::BeginPlay()
{
	Super::BeginPlay();

	PlayerHealth = GetOwner()->FindComponentByClass<UHealth>();
}

void URespawnPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (isDissapearing && PlayerSkeleton)
	{
		// You're getting sent to Brazil for this, Kalvin.
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(paramIncrease));
		DissolveValue -= DeltaTime * DissolveSpeed;
		PlayerSkeleton->SetScalarParameterValueOnMaterials(FName(*DissolveKey), DissolveValue);
		PlayerSkeleton->SetVectorParameterValueOnMaterials(FName(*DissolveColourKey), FVector(FadeColour));
	}

	if (PlayerHealth->CurrentHealth <= 0 && !isRespawning)
	{
		RespawnDelay();
		
		Death();

		isRespawning = true;
	}
}

void URespawnPlayer::DestroyPawn_Implementation()
{
	GetOwner()->Destroy();
}

void URespawnPlayer::Death_Implementation()
{
	USkeletalMeshComponent* PlayerHand = Cast<APlayerMovement>(GetOwner())->HandMesh;
	PlayerSkeleton = Cast<APlayerMovement>(GetOwner())->GetMesh();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UWeaponSwitching* WeaponSwitchingComponent = GetOwner()->FindComponentByClass<UWeaponSwitching>();
	UCameraComponent* CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	USkatingMovementComponent* SkatingMovement = Cast<APlayerMovement>(GetOwner())->GetSkatingMovementComponent();

	TArray<UShapeComponent*> ColliderComponents;
	GetOwner()->GetComponents(ColliderComponents);

	for (UShapeComponent* Actor : ColliderComponents)
	{
		Actor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	for (int i = 0; i < WeaponSwitchingComponent->EquipedWeapons.Num(); i++)
	{
		if (WeaponSwitchingComponent->EquipedWeapons[i] != nullptr)
		{
			WeaponSwitchingComponent->EquipedWeapons[i]->StopFire();
			WeaponSwitchingComponent->Drop(i);
		}
	}

	//Audio Bit here.
	DeathAudioTrigger();

	PlayerSkeleton->SetSkeletalMesh(CorpseMesh);
	PlayerSkeleton->SetLinearDamping(5000);
	PlayerSkeleton->SetAngularDamping(5000);
	PlayerSkeleton->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PlayerSkeleton->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	PlayerSkeleton->SetSimulatePhysics(true);
	PlayerSkeleton->SetOwnerNoSee(false);

	inst = GetWorld()->GetParameterCollectionInstance(CorpseMaterialParameter);
	DissolveValue = 1.0f;
	isDissapearing = true;
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->AttachToComponent(PlayerSkeleton, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	PlayerHand->DestroyComponent();
	SkatingMovement->GravityScale = 0;
	SkatingMovement->Velocity = FVector::ZeroVector;
	GetOwner()->DisableInput(PlayerController);

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
		GetWorld()->GetTimerManager().SetTimer(HealthSpawnTimerHandle, this, &URespawnPlayer::DropHealthKit, 0.2f, false, 0.2f);

	GetWorld()->GetTimerManager().SetTimer(DestroyPawnTimerHandle, this, &URespawnPlayer::DestroyPawn, DestroyPawnDelayTime, false);

	isRespawning = true;

	// Bit of a botched fix for now...
	// and probably forever.
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn)
	{
		AMainPlayerState* state = pawn->GetPlayerState<AMainPlayerState>();
		if (state)
			state->IsAlive = false;
	}

	// For any UI wackiness this is where to look.
	// This was made since when switching team the remove-ui isn't called until the death has finished.
	if (UGeneral::IsOwnedBy(this->GetOwner(), UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
}

void URespawnPlayer::DropHealthKit() 
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(HealthDropObject, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation(), SpawnParameters);
}

void URespawnPlayer::RespawnDelay_Implementation()
{
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn)
	{
		AFPSController* controller = pawn->GetController<AFPSController>();
		if (controller)
			GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, controller, &AFPSController::RequestCameraSpectator,
												   RespawnDelayTime, false);
	}
}

void URespawnPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URespawnPlayer , isDissapearing);
}

void URespawnPlayer::DeathAudioTrigger_Implementation()
{
	//Audio Implementation Here.
}