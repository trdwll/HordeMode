// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Base/HMFirearmBase.h"

// Sets default values
AHMFirearmBase::AHMFirearmBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHMFirearmBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHMFirearmBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

