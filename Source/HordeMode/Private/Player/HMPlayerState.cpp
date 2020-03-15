// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Player/HMPlayerState.h"
#include "Net/UnrealNetwork.h"

AHMPlayerState::AHMPlayerState()
	: m_Kills(0), m_Deaths(0)
{

}

void AHMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMPlayerState, m_Kills);
	DOREPLIFETIME(AHMPlayerState, m_Deaths);
}

void AHMPlayerState::Reset()
{
	Super::Reset();

	m_Kills = 0;
	m_Deaths = 0;
}
