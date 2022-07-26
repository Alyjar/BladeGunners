// Fill out your copyright notice in the Description page of Project Settings.


#include "FireCameraShake.h"

UFireCameraShake::UFireCameraShake()
{
    OscillationDuration = 0.25f;
    OscillationBlendInTime = 0.05f;
    OscillationBlendOutTime = 0.05f;

    RotOscillation.Pitch.Amplitude = FMath::RandRange(0.2f, 0.7f);
    RotOscillation.Pitch.Frequency = FMath::RandRange(0.5f, 2.0f);

    RotOscillation.Yaw.Amplitude = FMath::RandRange(0.2f, 0.7f);
    RotOscillation.Yaw.Frequency = FMath::RandRange(0.5f, 3.0f);
}
