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
		PRINT("Unable to interact - The interactor isn't valid!");
		return;
	}

	if (m_Cost <= 0)
	{
		PRINT("Door has been purchased already.");
		return;
	}

	if (AHMPlayerState* const PS = Cast<AHMPlayerState>(Player->GetPlayerState()))
	{
		// TODO: Subtract from the players currency
	}

	PRINT("Cost Left: " + *FString::FromInt(m_Cost));
}


