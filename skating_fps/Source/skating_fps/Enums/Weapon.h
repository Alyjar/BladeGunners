#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeapon : uint8
{
    None,
    Pistol,
    AR,
    Shotgun,
    Sniper
};