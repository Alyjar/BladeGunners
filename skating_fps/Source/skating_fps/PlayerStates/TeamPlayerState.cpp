#include "TeamPlayerState.h"

#include "../Controllers/TDMController.h"

#include "Net/UnrealNetwork.h"

ATeamPlayerState::ATeamPlayerState()
{
	SetReplicates(true);
}

void ATeamPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATeamPlayerState, Team); 
	DOREPLIFETIME(ATeamPlayerState, UserName);
}

void ATeamPlayerState::Client_CloseTeamSelectWindow_Implementation()
{
	ATDMController* controller = GetWorld()->GetFirstPlayerController<ATDMController>();
	if (controller)
		controller->ForceCloseTeamSelect();
}
