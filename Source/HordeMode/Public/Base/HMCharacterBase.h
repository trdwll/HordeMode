// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HMCharacterBase.generated.h"

/**
 * The base class for all characters in the level
 * This class implements health etc to be used by all enemies and players
 */
UCLASS()
class HORDEMODE_API AHMCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHMCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
