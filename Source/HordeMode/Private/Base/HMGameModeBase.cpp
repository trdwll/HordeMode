// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMGameModeBase.h"
#include "Base/HMCharacterBase.h"
#include "HMCommon.h"
#include "Player/HMPlayerState.h"

void AHMGameModeBase::Killed(AController* Killer, AController* VictimPlayer)
{
	AHMPlayerState* const KillerPS = Cast<AHMPlayerState>(Killer->PlayerState);
	AHMPlayerState* const VictimPS = Cast<AHMPlayerState>(VictimPlayer->PlayerState);

	if (KillerPS != nullptr && KillerPS != VictimPS && !KillerPS->bIsABot)
	{
		KillerPS->AddKill();
	}

	if (VictimPS != nullptr && !VictimPS->bIsABot)
	{
		VictimPS->AddDeath();
	}
}

FString AHMGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	if (AHMPlayerState* const PlayerState = Cast<AHMPlayerState>(NewPlayerController->PlayerState))
	{
		PlayerState->ChangeTeamType(ETeamType::Player);
		PlayerState->bIsABot = false;
	}

	return result;
}
