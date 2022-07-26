#pragma once

class UMultiLineEditableTextBox;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatLine.generated.h"

UCLASS()
class SKATING_FPS_API UChatLine : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetText(FString text);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMultiLineEditableTextBox* TextBlock = nullptr;
};
