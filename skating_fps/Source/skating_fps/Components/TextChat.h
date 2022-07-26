#pragma once

#include "../Structs/PlayerChatMessage.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TextChat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageSignature, const FPlayerChatMessage&, message);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKATING_FPS_API UTextChat : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTextChat();

	UFUNCTION(BlueprintCallable, Server, UnReliable)
	void Server_SendMessage(FPlayerChatMessage message);

	UFUNCTION(BlueprintCallable, Client, UnReliable)
	void Client_SendMessage(FPlayerChatMessage message);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable)
	FMessageSignature OnMessageReceived = FMessageSignature();
};
