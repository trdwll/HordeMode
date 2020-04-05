// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMGameModeBase.h"
#include "HMCommon.h"
#include "Player/HMPlayerState.h"

FString AHMGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	if (AHMPlayerState* const PlayerState = Cast<AHMPlayerState>(NewPlayerController->PlayerState))
	{
		PlayerState->ChangeTeamType(ETeamType::Player);
	}

	return result;
}
