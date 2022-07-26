#pragma once

class UPopUpWindow;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuEventSignature);

UCLASS()
class SKATING_FPS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CycleQueue();
	UFUNCTION(BlueprintCallable)
	void AddToQueue(TArray<TSubclassOf<UPopUpWindow>> menus);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ClearQueue();

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FMenuEventSignature OnCancel = FMenuEventSignature();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UPopUpWindow* CurrentWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UPopUpWindow>> WidgetQueue = TArray<TSubclassOf<UPopUpWindow>>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UPopUpWindow>> ServerBrowser = TArray<TSubclassOf<UPopUpWindow>>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UPopUpWindow>> HostServer = TArray<TSubclassOf<UPopUpWindow>>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UPopUpWindow>> JoinServer = TArray<TSubclassOf<UPopUpWindow>>();
};
