#pragma once

class AController;

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitialisedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNamedInitialisedSignature, const FString&, name);

UCLASS(Config=Game)
class SKATING_FPS_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMainPlayerState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitialiseUserName();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UpdateUserName(const FString& name);

	UFUNCTION(BlueprintCallable)
	void SetSavedUserName(FString name);

	UFUNCTION(BlueprintCallable)
	FString GetSavedUserName();

	UFUNCTION(BlueprintCallable)
	void AddKill();
	UFUNCTION(BlueprintCallable)
	int GetKills();

	UFUNCTION(BlueprintCallable)
	void AddDeath();
	UFUNCTION(BlueprintCallable)
	int GetDeaths();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FString UserName = "";

	UPROPERTY(BlueprintAssignable)
	FNamedInitialisedSignature OnNameInitialised = FNamedInitialisedSignature();

	UPROPERTY(BlueprintAssignable)
	FInitialisedSignature OnInitialised = FInitialisedSignature();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = true))
	bool IsAlive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = true))
	int Kills = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = true))
	int Deaths = 0;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString UnknownUserName = "Guest";
};
