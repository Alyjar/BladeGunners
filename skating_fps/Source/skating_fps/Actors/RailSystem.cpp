#include "RailSystem.h"

#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"

ARailSystem::ARailSystem()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(GetRootComponent());
}

int ARailSystem::GetClosestSplinePoint(FVector location)
{
	if (!Spline)
		return -1;

	int index = 0;
	float closestDistance = MAX_FLT;
	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		float distance = FVector::Dist(location, Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
		if (distance <= closestDistance)
		{
			closestDistance = distance;
			index = i;
		}
	}

	return index;
}

float ARailSystem::GetTimeAtLocation(FVector location, float accuracy)
{
	if (!Spline)
		return -1.0f;

	float time = -1.0f;
	float closestDistance = MAX_FLT;
	for (float i = 0; i <= 1.0f; i += accuracy)
	{
		FVector locationAtTime = Spline->GetLocationAtTime(i, ESplineCoordinateSpace::World);
		float distance = FVector::Dist(locationAtTime, location);
		if (distance <= closestDistance)
		{
			time = i;
			closestDistance = distance;
		}
	}

	return time;
}

float ARailSystem::GetDistanceToNextPoint(int direction, FVector currentLocation)
{
	int currentSpline = GetClosestSplinePoint(currentLocation);

	int nextIndex = (currentSpline + direction);
	nextIndex = FMath::Clamp(nextIndex, 0, Spline->GetNumberOfSplinePoints());

	FVector previousLocation = Spline->GetLocationAtSplinePoint(currentSpline, ESplineCoordinateSpace::World);
	FVector nextLocation = Spline->GetLocationAtSplinePoint(nextIndex, ESplineCoordinateSpace::World);

	return FVector::Dist(previousLocation, nextLocation);
}

void ARailSystem::BeginPlay()
{
	Super::BeginPlay();
}

void ARailSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

