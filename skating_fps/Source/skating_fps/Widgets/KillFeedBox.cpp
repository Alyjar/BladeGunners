#include "KillFeedBox.h"

#include "../FunctionLibraries/General.h"
#include "../GameStates/MainLevelGameState.h"
#include "../PlayerStates/TeamPlayerState.h"

#include "Components/TextBlock.h"

void UKillFeedBox::NativeConstruct()
{
	AMainLevelGameState* gs = GetWorld()->GetGameState<AMainLevelGameState>();
	if (gs)
	{
		CyanColour = gs->GetTeamColour(EPlayerTeam::Cyan);
		PinkColour = gs->GetTeamColour(EPlayerTeam::Pink);
	}

	Super::NativeConstruct();
}

void UKillFeedBox::SetInitiator_Implementation(const AMainPlayerState* player)
{
}

void UKillFeedBox::SetVictim_Implementation(const AMainPlayerState* player)
{
}

void UKillFeedBox::SetWeaponIcon_Implementation(const EWeapon& weapon)
{
}

void UKillFeedBox::SetHeadshotIcon_Implementation(bool value)
{

}

void UKillFeedBox::SetTextBoxValues(FString name, const AMainPlayerState* player, UTextBlock* text)
{
	if (!text)
		return;

	text->SetText(FText::FromString(name));

	FColor teamColour = DefaultColour;

	const ATeamPlayerState* teamState = Cast<ATeamPlayerState>(player);
	if (teamState)
	{
		switch (teamState->Team.Team)
		{
		case EPlayerTeam::Cyan:
			teamColour = CyanColour;
			break;
		case EPlayerTeam::Pink:
			teamColour = PinkColour;
			break;
		}
	}

	FSlateColor colour = FSlateColor(teamColour);
	text->SetColorAndOpacity(colour);
}

FString UKillFeedBox::GetShortenedName(FString name)
{
	if (name.Len() > MaxNameLimit)
	{
		FString shortened = (UGeneral::ClampText(FText::FromString(name), MaxNameLimit + 3)).ToString();
		return shortened + "...";
	}

	else
		return name;
}