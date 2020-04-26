// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Player/HMPlayerState.h"
#include "HMCommon.h"
#include "HordeMode.h"

#include "Net/UnrealNetwork.h"

AHMPlayerState::AHMPlayerState()
	: m_TeamType(ETeamType::Enemy), m_Currency(1000), m_Kills(0), m_Deaths(0)
{
	bIsABot = true;
}

void AHMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMPlayerState, m_TeamType);
	DOREPLIFETIME(AHMPlayerState, m_Currency);
	DOREPLIFETIME(AHMPlayerState, m_Kills);
	DOREPLIFETIME(AHMPlayerState, m_Deaths);
}

void AHMPlayerState::Reset()
{
	Super::Reset();

	m_Currency = 0;
	m_Kills = 0;
	m_Deaths = 0;
}

void AHMPlayerState::SubtractCurrency(int32 CurrencyToSubtract)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SubtractCurrency(CurrencyToSubtract);
	}

	m_Currency -= FMath::Clamp(m_Currency, 0, CurrencyToSubtract);

	OnCharacterCurrencyChange.Broadcast(m_Currency);
}

void AHMPlayerState::AddCurrency(int32 CurrencyToAdd)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_AddCurrency(CurrencyToAdd);
	}

	m_Currency += CurrencyToAdd;

	OnCharacterCurrencyChange.Broadcast(m_Currency);
}

void AHMPlayerState::SetCurrency(int32 NewCurrency)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetCurrency(NewCurrency);
	}

	m_Currency = FMath::Clamp(m_Currency, 0, NewCurrency);

	OnCharacterCurrencyChange.Broadcast(m_Currency);
}

bool AHMPlayerState::Server_AddCurrency_Validate(int32 CurrencyToAdd) { return true; }
void AHMPlayerState::Server_AddCurrency_Implementation(int32 CurrencyToAdd) { AddCurrency(CurrencyToAdd); }

bool AHMPlayerState::Server_SubtractCurrency_Validate(int32 CurrencyToSubtract) { return true; }
void AHMPlayerState::Server_SubtractCurrency_Implementation(int32 CurrencyToSubtract) { SubtractCurrency(CurrencyToSubtract); }

bool AHMPlayerState::Server_SetCurrency_Validate(int32 NewCurrency) { return true;}
void AHMPlayerState::Server_SetCurrency_Implementation(int32 NewCurrency) { SetCurrency(NewCurrency); }
