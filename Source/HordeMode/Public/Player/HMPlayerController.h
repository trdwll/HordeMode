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

    void RegisterRecoil(float Horizontal, float Vertical);
    void ResetRecoil();

};
