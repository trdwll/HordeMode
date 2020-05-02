// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HMPlayerController.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:

    /** Pitch is Y and Yaw is Z - X is Roll */
    void RegisterRecoil(float Pitch, float Yaw);
    void ResetRecoil();

};
