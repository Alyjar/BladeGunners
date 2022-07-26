#pragma once

#include "../Structs/ServerInfo.h"
#include "FindSessionsCallbackProxy.h"

class UButton;
class UTextBlock;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerInfoSignature, const FServerInfo&, info);

UCLASS()
class SKATING_FPS_API USessionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetGraphics();

	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	int ServerNameMaxLength = 12;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Networking", meta = (AllowPrivateAccess = "true"))
	FServerInfo Server = FServerInfo();

	UPROPERTY(BlueprintAssignable)
	FServerInfoSignature OnClickedInfo = FServerInfoSignature();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	UButton* Button = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UTextBlock* NameBlock = nullptr;
};
