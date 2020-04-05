// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HMAIController.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMAIController final : public AAIController
{
	GENERATED_BODY()

public:
    AHMAIController(const class FObjectInitializer& ObjectInitializer);
};
