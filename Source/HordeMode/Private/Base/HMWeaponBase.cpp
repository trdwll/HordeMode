// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMWeaponBase.h"
#include "Net/UnrealNetwork.h"

AHMWeaponBase::AHMWeaponBase() : m_CurrentAttachLocation(EWeaponAttachLocation::Hands), m_CurrentAmmo(0), m_CurrentAmmoInMag(0)
{
	PrimaryActorTick.bCanEverTick = true;

	m_WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = m_WeaponMesh;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void AHMWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHMWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHMWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHMWeaponBase, m_CurrentAmmo, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHMWeaponBase, m_CurrentAmmoInMag, COND_SkipOwner);
}

