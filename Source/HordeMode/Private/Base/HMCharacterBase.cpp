// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMCharacterBase.h"
#include "Components/HMHealthComponent.h"

// Sets default values
AHMCharacterBase::AHMCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_HealthComponent = CreateDefaultSubobject<UHMHealthComponent>(TEXT("HMHealthComponent"));
}

// Called when the game starts or when spawned
void AHMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


