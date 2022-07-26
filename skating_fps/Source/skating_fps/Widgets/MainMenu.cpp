#include "MainMenu.h"

#include "../Widgets/PopUpWindow.h"

void UMainMenu::CycleQueue()
{
	if (CurrentWidget)
		CurrentWidget->RemoveFromParent();

	if (WidgetQueue.Num() > 0)
	{
		CurrentWidget = CreateWidget<UPopUpWindow>(this, WidgetQueue[0]);
		CurrentWidget->AddToViewport(1);

		CurrentWidget->OnSuccess.AddDynamic(this, &UMainMenu::CycleQueue);
		CurrentWidget->OnFail.AddDynamic(this, &UMainMenu::ClearQueue);

		WidgetQueue.RemoveAt(0);
	}
}

void UMainMenu::AddToQueue(TArray<TSubclassOf<UPopUpWindow>> menus)
{
	WidgetQueue.Append(menus);
}

void UMainMenu::ClearQueue_Implementation()
{
	WidgetQueue.Empty();
	CurrentWidget = nullptr;
}