// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Player/HMPlayerController.h"

void AHMPlayerController::RegisterRecoil(float Pitch, float Yaw)
{
	AddYawInput(Yaw);
	AddPitchInput(Pitch);
}

void AHMPlayerController::ResetRecoil()
{
	AddYawInput(0.0f);
	AddPitchInput(0.0f);
}
