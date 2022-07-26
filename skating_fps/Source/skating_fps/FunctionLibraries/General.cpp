#include "General.h"

#include "Components/TextBlock.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

FText UGeneral::ClampText(FText text, int MaxMessageSize)
{
	FString str = text.ToString();
	if (str.Len() > MaxMessageSize)
		str.RemoveAt(MaxMessageSize, str.Len() + MaxMessageSize);

	return FText::FromString(str);
}

bool UGeneral::IsOwnedBy(AActor* actor, APlayerController* controller)
{
	if (actor && controller)
		return actor->IsOwnedBy(controller);

	return false;
}

void UGeneral::JoinIP(APlayerController* controller, FString ip)
{
	if (controller)
	{
		controller->ClientTravel(ip, ETravelType::TRAVEL_Absolute, false);
	}
}

void UGeneral::SetTextTime(FString copyrightText, UTextBlock* text)
{
	FDateTime date = FDateTime::Now();
	FString hours = (date.GetHour() < 10) ? "0" + FString::FromInt(date.GetHour()) : FString::FromInt(date.GetHour());
	FString minutes = (date.GetMinute() < 10) ? "0" + FString::FromInt(date.GetMinute()) : FString::FromInt(date.GetMinute());

	FString str = copyrightText + "\n" + hours + ":" + minutes;
	text->SetText(FText::FromString(str));
}

FString UGeneral::TimeToText(int minutes, int seconds)
{
	FString minStr = (minutes < 10) ? "0" + FString::FromInt(minutes) : FString::FromInt(minutes);
	FString secStr = (seconds < 10) ? "0" + FString::FromInt(seconds) : FString::FromInt(seconds);

	return minStr + ":" + secStr;
}