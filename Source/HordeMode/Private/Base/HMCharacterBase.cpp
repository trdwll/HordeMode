// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "HordeMode.h"

AHMCharacterBase::AHMCharacterBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_Health(100.0f), m_MaxHealth(100.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AHMCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMCharacterBase, m_Health);
}

float AHMCharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	PRINT("TOOK DAMAGE - " + FString::SanitizeFloat(ActualDamage));

	// TODO: clamp this later - just for testing right now
	m_Health -= ActualDamage;

	return ActualDamage;
}

void AHMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


