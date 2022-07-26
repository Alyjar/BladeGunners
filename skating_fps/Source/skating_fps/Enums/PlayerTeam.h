#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "PlayerTeam.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
    None,
    Cyan,
    Pink,
    Spectator,
};