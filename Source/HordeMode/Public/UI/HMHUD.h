// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HMHUD.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMHUD : public AHUD
{
	GENERATED_BODY()

public:
	AHMHUD();
	virtual void DrawHUD() override;
};
