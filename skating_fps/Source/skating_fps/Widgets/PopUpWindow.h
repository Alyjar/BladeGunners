#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopUpWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUXSignature);

UCLASS()
class SKATING_FPS_API UPopUpWindow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FUXSignature OnSuccess = FUXSignature();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FUXSignature OnFail = FUXSignature();
};
