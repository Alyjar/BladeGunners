#include "TextChat.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UTextChat::UTextChat()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Bit of a costly call, but removes the dependency of the LobbyPawn.
void UTextChat::Server_SendMessage_Implementation(FPlayerChatMessage message)
{
	TArray<AActor*> actors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		UTextChat* chat = actor->FindComponentByClass<UTextChat>();
		if (chat)
			chat->Client_SendMessage(message);
	}
}

void UTextChat::Client_SendMessage_Implementation(FPlayerChatMessage message)
{
	OnMessageReceived.Broadcast(message);
}

void UTextChat::BeginPlay()
{
	Super::BeginPlay();
}
