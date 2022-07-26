#pragma once

class AActor;
class APlayerController;
class UTextBlock;

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "General.generated.h"

UCLASS()
class SKATING_FPS_API UGeneral : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static FText ClampText(FText text, int MaxMessageSize);

	// Make sure the Player Controller uses GetPlayerControllerFromID -> NOT THE PAWN'S CONTROLLER
	UFUNCTION(BlueprintCallable)
	static bool IsOwnedBy(AActor* actor, APlayerController* controller);

	UFUNCTION(BlueprintCallable)
	static void JoinIP(APlayerController* controller, FString ip);

	UFUNCTION(BlueprintCallable)
	static void SetTextTime(FString copyrightText, UTextBlock* text);

	UFUNCTION(BlueprintCallable)
	static FString TimeToText(int minutes, int seconds);
};
