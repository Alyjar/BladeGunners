#include "ChatLine.h"

#include "Components/MultiLineEditableTextBox.h"

void UChatLine::NativeConstruct()
{
	Super::NativeConstruct();
}

void UChatLine::SetText(FString message)
{
	if (TextBlock)
		TextBlock->SetText(FText::FromString(message));
}

