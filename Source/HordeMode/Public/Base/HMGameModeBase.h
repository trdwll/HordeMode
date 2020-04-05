// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HMGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()



protected:
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
};
