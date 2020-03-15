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
	AHMCharacterBase(const class FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	/** The health component to allow this character to be damaged. */
	UPROPERTY(VisibleAnywhere, Category = "HMCharacterBase", meta = (DisplayName = "HMHealthComponent"))
	class UHMHealthComponent* m_HealthComponent;


public:
	/** Get the health component of this character. */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE class UHMHealthComponent* GetHealthComponent() const { return m_HealthComponent; }
};
