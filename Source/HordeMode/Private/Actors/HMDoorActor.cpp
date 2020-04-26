// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Actors/HMDoorActor.h"
#include "HordeMode.h"

#include "Player/HMPlayerCharacter.h"
#include "Player/HMPlayerState.h"

AHMDoorActor::AHMDoorActor() : m_Cost(1000)
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AHMDoorActor::BeginPlay()
{
	Super::BeginPlay();
}

void AHMDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHMDoorActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMDoorActor, m_Cost);
}

void AHMDoorActor::Interact_Implementation(AHMPlayerCharacter* Player)
{
	if (Player == nullptr)
	{
		return;
	}

	if (AHMPlayerState* const PS = Cast<AHMPlayerState>(Player->GetPlayerState()))
	{
		int32 CostPaid = FMath::Clamp(PS->GetCurrency(), 0, m_Cost);

		if (CostPaid > 0)
		{
			PS->SubtractCurrency(CostPaid);

			m_Cost -= CostPaid;

			m_OnDoorPayToward.Broadcast(PS, CostPaid);
		}
	}

	if (m_Cost <= 0)
	{
		m_OnDoorPurchased.Broadcast();
		return;
	}
}


