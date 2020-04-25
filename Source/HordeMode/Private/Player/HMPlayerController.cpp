// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Player/HMPlayerController.h"

void AHMPlayerController::RegisterRecoil(float Horizontal, float Vertical)
{
	AddYawInput(Horizontal);
	AddPitchInput(Vertical);
}

void AHMPlayerController::ResetRecoil()
{
	AddYawInput(0.0f);
	AddPitchInput(0.0f);
}
