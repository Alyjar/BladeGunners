#pragma once

#include "../Enums/PlayerTeam.h"

#include "Math/Color.h"

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "TeamData.generated.h"

USTRUCT(BlueprintType)
struct FTeamData
{
	GENERATED_BODY()

	FTeamData() {};

	FTeamData(EPlayerTeam team) : Team(team) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerTeam Team = EPlayerTeam::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor Colour = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPlayers = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Kills = 0;
};
