#include "OptionsMenu.h"

#include "GameFramework/InputSettings.h"
#include "../GameInstances/SteamGlobalGameInstance.h"

void UOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MouseSensitivity = GetMouseSensitivity();
}

void UOptionsMenu::SetMasterSound(float value)
{
	float clamped = FMath::Clamp(value, 0.0f, 1.0f);
	
	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		instance->SetMasterVolume(clamped);
}

float UOptionsMenu::GetMasterSound()
{
	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		return instance->GetMasterVolume();

	return -1.0f;
}

void UOptionsMenu::SetMusicSound(float value)
{
	float clamped = FMath::Clamp(value, 0.0f, 1.0f);

	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		instance->SetMusicVolume(clamped);
}

float UOptionsMenu::GetMusicSound()
{
	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		return instance->GetMusicVolume();

	return -1.0f;
}

void UOptionsMenu::SetVFXSound(float value)
{
	float clamped = FMath::Clamp(value, 0.0f, 1.0f);

	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		instance->SetVFXVolume(clamped);
}

float UOptionsMenu::GetVFXSound()
{
	USteamGlobalGameInstance* instance = Cast<USteamGlobalGameInstance>(GetWorld()->GetGameInstance());
	if (instance)
		return instance->GetVFXVolume();

	return -1.0f;
}

void UOptionsMenu::SetMouseSensitivity(float value)
{
	UInputSettings* input = UInputSettings::GetInputSettings();
	if (!input || value == 0.0f)
		return;

	FInputAxisKeyMapping mouseX = GetFirstAxisMapping(MouseXName);
	FInputAxisKeyMapping mouseY = GetFirstAxisMapping(MouseYName);

	input->RemoveAxisMapping(mouseX, false);
	input->RemoveAxisMapping(mouseY, false);

	float xScale = mouseX.Scale;
	float yScale = mouseY.Scale;

	mouseX.Scale = (FMath::Abs(xScale) / xScale) * value;
	mouseY.Scale = (FMath::Abs(yScale) / yScale) * value;

	input->AddAxisMapping(mouseX, false);
	input->AddAxisMapping(mouseY, true);

	MouseSensitivity = value;

	input->SaveConfig();
}

void UOptionsMenu::SetMouseXInput(bool isInverted)
{
	UInputSettings* input = UInputSettings::GetInputSettings();
	if (!input)
		return;

	FInputAxisKeyMapping xMapping = GetFirstAxisMapping(MouseXName);
	input->RemoveAxisMapping(xMapping, false);

	xMapping.Scale = isInverted ? -MouseSensitivity : MouseSensitivity;
	input->AddAxisMapping(xMapping, true);

	input->SaveConfig();
}

void UOptionsMenu::SetMouseYInput(bool isInverted)
{
	UInputSettings* input = UInputSettings::GetInputSettings();
	if (!input)
		return;
	FInputAxisKeyMapping yMapping = GetFirstAxisMapping(MouseYName);
	input->RemoveAxisMapping(yMapping, false);

	// Non-inverted Y mouse is a -1.0f.
	yMapping.Scale = isInverted ? MouseSensitivity : -MouseSensitivity;
	input->AddAxisMapping(yMapping, true);

	input->SaveConfig();
}

float UOptionsMenu::GetMouseSensitivity()
{
	FInputAxisKeyMapping xMapping = GetFirstAxisMapping(MouseXName);
	return FMath::Abs(xMapping.Scale);
}

bool UOptionsMenu::IsMouseXInverted()
{
	FInputAxisKeyMapping key = GetFirstAxisMapping(MouseXName);
	return key.Scale < 0.0f;
}

bool UOptionsMenu::IsMouseYInverted()
{
	FInputAxisKeyMapping key = GetFirstAxisMapping(MouseYName);
	// -1.0f on the Y is regular non-inverted.
	return key.Scale > 0.0f;
}

FIntPoint UOptionsMenu::GetResolutionFromString(FString resolution)
{
	FString width = "";
	FString height = "";

	bool hasWidth = false;

	for (TCHAR c : resolution.ToLower())
	{
		if (c == ' ')
			continue;

		if (c == 'x')
		{
			hasWidth = true;
			continue;
		}

		if (!hasWidth)
			width += c;

		else
			height += c;

	}

	int x = FCString::Atoi(*width);
	int y = FCString::Atoi(*height);

	return FIntPoint(x, y);
}

FInputAxisKeyMapping UOptionsMenu::GetFirstAxisMapping(FString value)
{
	TArray<FInputAxisKeyMapping> keys = TArray<FInputAxisKeyMapping>();
	UInputSettings* input = UInputSettings::GetInputSettings();
	
	// Should almost never be the case.
	if (!input)
		return {};

	input->GetAxisMappingByName(FName(value), keys);

	if (keys.Num() > 0)
		return keys[0];

	return {};
}
