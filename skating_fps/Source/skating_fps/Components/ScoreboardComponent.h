#pragma once

class UScoreboard;

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScoreboardComponent.generated.h"

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SKATING_FPS_API UScoreboardComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UScoreboardComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreateScoreboard();

	UFUNCTION(BlueprintCallable)
	void ToggleScoreboard();

	UFUNCTION(BlueprintCallable)
	void OpenScoreboard();

	UFUNCTION(BlueprintCallable)
	void CloseScoreboard();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UScoreboard> ScoreboardTemplate = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UScoreboard* ScoreboardInstance = nullptr;
};
