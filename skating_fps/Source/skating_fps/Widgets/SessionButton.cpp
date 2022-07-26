#include "SessionButton.h"

#include "../FunctionLibraries/General.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionButton::SetGraphics()
{
	FString name = UGeneral::ClampText(FText::FromString(Server.ServerName), ServerNameMaxLength).ToString();
	FString str = name+ " / " + Server.ServerIP;
	NameBlock->SetText(FText::FromString(str));
}

void USessionButton::OnButtonPressed()
{
	OnClickedInfo.Broadcast(Server);
}
