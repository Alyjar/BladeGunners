#include "GrindingSystem.h"

#include "../Actors/RailSystem.h"
#include "../Movement/PlayerMovement.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

typedef UCharacterMovementComponent CM;

UGrindingSystem::UGrindingSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetIsReplicatedByDefault(true);

	if (GetOwner())
	{
		DetectionTransform = CreateDefaultSubobject<USceneComponent>(TEXT("Detection Transform"));
		DetectionTransform->SetupAttachment(GetOwner()->GetRootComponent());

		DetectionTransform->AddRelativeLocation(DetectionOffset);
	}

}

void UGrindingSystem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGrindingSystem, AttachedRail);
}

bool UGrindingSystem::IsAttached()
{
	return AttachedRail != nullptr;
}

void UGrindingSystem::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		// Client has authority on position otherwise it'll cause inconsistencies when jumping off.
		// It'll also make the whole experience smoother client side.
		// Server side will still suck though.
		// Future Chris: Turns out it does not suck.
		CM* movement = Cast<CM>(GetOwner()->GetComponentByClass(CM::StaticClass()));
		if (movement)
		{
			movement->bIgnoreClientMovementErrorChecksAndCorrection = true;
			movement->bServerAcceptClientAuthoritativePosition = true;
		}
	}
}

void UGrindingSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Allows the re-joining of the rail once they go downwards.
	if (!CanRejoin && GetOwner()->GetVelocity().Z <= MaxKickOffReturnVelocity)
		CanRejoin = true;

	APlayerController* controller = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
	if (controller)
	{
		// Staying on the Rail.
		if (controller->IsInputKeyDown(EKeys::SpaceBar))
		{
			// No point in searching for a rail while already on it.
			if (!IsAttached() && CanRejoin)
				CheckForSpline();
		}

		// Key is no longer Pressed and is on a Rail.
		else if (IsAttached())
			LaunchOffRail();
	}

	// Will only change the time and set the position if it is controlled by the client.
	if (IsAttached() && GetOwner()->IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
	{
		AutoIncreaseVelocity(DeltaTime);
		AutoIncreaseTime(DeltaTime, AttachedRail);

		MoveAlongRail(DeltaTime, AttachedRail);
		CheckForEnd();

		Server_SetPosition(GetOwner()->GetActorLocation());
	}

	// Ensure this is last as it's resetting the Input value for every frame.
	InputDir = FVector::ZeroVector;
}

void UGrindingSystem::AutoIncreaseVelocity(float DeltaTime)
{
	VelocityScale += VelocityScaleIncrease * DeltaTime;
	VelocityScale = FMath::Clamp(VelocityScale, 0.0f, 1.0f);
}

void UGrindingSystem::AutoIncreaseTime(float DeltaTime, ARailSystem* rail)
{
	// Only the owning player can change the time.
	if (!GetOwner()->IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
		return;

	float direction = LastDirection;

	if (direction != 0 && FMath::Abs(direction) >= AimTolerance)
	{
		int scalar = FMath::RoundToInt(FMath::Abs(direction) / direction);
		float value = ((float)scalar * DeltaTime * VelocityScale) * AttachedRail->GrindSpeed;
		CurrentTime += value;
		CurrentTime = FMath::Clamp(CurrentTime, 0.0f, 1.0f);

		Server_UpdateTime(CurrentTime);
	}
}

void UGrindingSystem::InputIncreaseTime(float DeltaTime, ARailSystem* rail, const FVector input)
{
	// Only the owning player can change the time.
	if (!GetOwner()->IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)) || input.IsNearlyZero())
		return;


	float dot = GetDirectionOnSpline(rail);
	float direction = input.X * LastDirection;

	CurrentTime += GrindAddAcceleration * DeltaTime * direction;
	Server_UpdateTime(CurrentTime);
}

float UGrindingSystem::GetDirectionOnSpline(ARailSystem* rail)
{
	int numSplines = rail->Spline->GetNumberOfSplinePoints();
	int index = rail->GetClosestSplinePoint(GetOwner()->GetActorLocation());

	// Upper refers to the Spline Index + 1.
	// This is because I'm trying to find the upper and lower relative
	// to the player with previous and next index.

	// Just a sanity check to make it in range.
	// If it's not in range then set it to be the current index.
	int upperIndex = (index + 1 >= numSplines) ? -1 : index + 1;

	// Next index is the one in front of the Player.
	FVector upperDir = rail->Spline->GetDirectionAtSplinePoint(upperIndex, ESplineCoordinateSpace::World);
	float upperDot = FVector::DotProduct(upperDir, GetOwner()->GetActorForwardVector());

	return upperDot;
}

void UGrindingSystem::SetVerticalInput(float value)
{
	InputDir.X = value;
}

void UGrindingSystem::SetHorizontalInput(float value)
{
	InputDir.Y = value;
}

void UGrindingSystem::CheckForSpline()
{
	FVector ownerLocation = DetectionTransform->GetComponentLocation();

	TArray<FHitResult> hits = TArray<FHitResult>();

	FCollisionShape collisionSphere = FCollisionShape::MakeSphere(DetectionRadius);

	GetWorld()->SweepMultiByChannel(hits, ownerLocation, ownerLocation, FQuat::Identity, DetectionChannel, collisionSphere);

	TArray<ARailSystem*> rails = TArray<ARailSystem*>();
	for (FHitResult& hit : hits)
	{
		ARailSystem* rail = Cast<ARailSystem>(hit.Actor);
		if (rail)
			rails.Add(rail);
	}

	// Can do some sort of weighting priority here if there are multiple rails.
	// For now I doubt it'll be an issue.
	if (rails.Num() > 0)
	{
		for (ARailSystem* rail : rails)
		{
			FVector location = rail->Spline->FindLocationClosestToWorldLocation(
				GetOwner()->GetActorLocation(), ESplineCoordinateSpace::World);
			FVector directionToRail = (DetectionTransform->GetComponentLocation() - location).GetSafeNormal();

			// Dot Product here means you can't go from under it.
			float dot = FVector::DotProduct(directionToRail, rail->GetActorUpVector());

			if (dot > VerticalDetectionLeniency)
			{
				AttachedRail = rail;

				PostAttachEvent(); //Audio Attach sound here.
			}
		}
	}

	else
		AttachedRail = nullptr;


	if (AttachedRail)
	{
		Server_SetAttachedRail(AttachedRail);

		// Not using a Round since I need it to be either -1 or 1 and not 0.
		LastDirection = GetDirectionOnSpline(AttachedRail) > 0 ? 1 : -1;
		CurrentTime = AttachedRail->GetTimeAtLocation(GetOwner()->GetActorLocation(), DetectionAccuracy);

		// Sets the initial velocity when they've got on the rail from a scale of 0-1.
		APlayerMovement* player = Cast<APlayerMovement>(GetOwner());
		if (player)
		{
			FVector vel = GetOwner()->GetVelocity();
			vel.Z = 0.0f;

			VelocityScale = vel.Size() / player->GetMaxSpeed();
		}
	}
}

void UGrindingSystem::MoveAlongRail(float DeltaTime, ARailSystem* rail)
{
	FVector railExtents = GetRailExtents(rail) * 0.5f;

	FVector currentLocation = GetOwner()->GetActorLocation();
	FVector targetLocation = rail->Spline->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World);
	targetLocation.Z += railExtents.Z + AttachOffset.Z;

	GetOwner()->SetActorLocation(targetLocation, false, nullptr, ETeleportType::TeleportPhysics);

	FVector startPos = rail->Spline->GetLocationAtTime(0.0f, ESplineCoordinateSpace::World);
	startPos.Z += railExtents.Z;

	FVector endPos = rail->Spline->GetLocationAtTime(1.0f, ESplineCoordinateSpace::World);
	endPos.Z += railExtents.Z;

	FVector skateLocation = DetectionTransform->GetComponentLocation();
	float time = rail->GetTimeAtLocation(skateLocation);
}

void UGrindingSystem::Server_UpdateTime_Implementation(float time)
{
	CurrentTime = time;
	Multi_UpdateTime(CurrentTime);
}

void UGrindingSystem::Multi_UpdateTime_Implementation(float time)
{
	if (!(GetOwnerRole() == ROLE_AutonomousProxy || GetOwnerRole() == ROLE_Authority))
		CurrentTime = time;
}

void UGrindingSystem::Server_SetPosition_Implementation(FVector position)
{
	GetOwner()->SetActorLocation(position);
}

void UGrindingSystem::Server_SetAttachedRail_Implementation(ARailSystem* rail)
{
	AttachedRail = rail;
}

void UGrindingSystem::CheckForEnd()
{
	bool positive = ((LastDirection == 1) && (CurrentTime >= 1.0f - RailLeaveLeniency));
	bool negative = ((LastDirection == -1) && (CurrentTime <= 0.0f + RailLeaveLeniency));

	if (positive || negative)
	{
		CanRejoin = false;
		LaunchOffRail();
	}
}

void UGrindingSystem::LaunchOffRail()
{
	APlayerMovement* player = Cast<APlayerMovement>(GetOwner());
	if (player)
	{
		AttachedRail = nullptr;
		Server_SetAttachedRail(nullptr);

		FVector dir = GetOwner()->GetActorForwardVector();
		dir.Z = VerticalJumpModifier;

		float launchSpeed = FMath::Lerp(player->GetMinSpeed(), player->GetMaxSpeed(), VelocityScale);
		player->LaunchCharacter(dir * launchSpeed, true, true);
		
		VelocityScale = 0.0f;

		PostDetachEvent(); //Audio Event for Detaching Here.
	}
}

FVector UGrindingSystem::GetRailExtents(ARailSystem* rail)
{
	return rail->MeshUsed->GetBoundingBox().GetSize();
}

void UGrindingSystem::PostAttachEvent()
{
	//Audio Function.
	FOnAkPostEventCallback nullCallback;
	AttachPlayingId = UAkGameplayStatics::PostEvent(AttachEvent, this->GetOwner(), int32(0), nullCallback, TArray<FAkExternalSourceInfo>());
}

void UGrindingSystem::PostDetachEvent()
{
	//Audio Function.
	FOnAkPostEventCallback nullCallback;
	DetachPlayingId = UAkGameplayStatics::PostEvent(DetachEvent, this->GetOwner(), int32(0), nullCallback, TArray<FAkExternalSourceInfo>());
}
