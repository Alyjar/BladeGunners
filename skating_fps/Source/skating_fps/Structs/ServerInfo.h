#pragma once

#include "CoreMinimal.h"
#include "ServerInfo.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

	FServerInfo() {};

	FServerInfo(FString serverName, FString IP) : ServerName(serverName), ServerIP(IP) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ServerName = "NULL";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ServerIP = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PlayerCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPlayers = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ping = 0;
	
};
