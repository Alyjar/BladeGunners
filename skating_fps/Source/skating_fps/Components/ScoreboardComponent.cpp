#include "ScoreboardComponent.h"

#include "../Widgets/Scoreboard.h"

UScoreboardComponent::UScoreboardComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UScoreboardComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UScoreboardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UScoreboardComponent::CreateScoreboard()
{
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn)
	{
		APlayerController* controller = pawn->GetController<APlayerController>();
		if (controller)
		{
			if (ScoreboardTemplate)
			{
				ScoreboardInstance = CreateWidget<UScoreboard>(controller, ScoreboardTemplate);
				ScoreboardInstance->AddToViewport();
			}

			else
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "SCOREBOARD TEMPLATE NOT ASSIGNED!");
		}
	}
}

void UScoreboardComponent::ToggleScoreboard()
{

}

void UScoreboardComponent::OpenScoreboard()
{
	if (ScoreboardInstance)
		ScoreboardInstance->SetVisibility(ESlateVisibility::Visible);

	else
		CreateScoreboard();
}

void UScoreboardComponent::CloseScoreboard()
{
	if (ScoreboardInstance)
		ScoreboardInstance->SetVisibility(ESlateVisibility::Hidden);
}