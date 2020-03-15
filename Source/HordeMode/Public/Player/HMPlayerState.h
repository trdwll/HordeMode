// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HMPlayerState.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMPlayerState : public APlayerState
{
	GENERATED_BODY()



public:
	AHMPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Reset() override;


	/** --- Start HMPlayerState code --- */
private:

	/** The number of kills that the player has. */
	UPROPERTY(Replicated)
	int32 m_Kills;

	/** The number of deaths that the player has. */
	UPROPERTY(Replicated)
	int32 m_Deaths;

	///** The number of downs that the player has. */
	//UPROPERTY(Replicated)
	//int32 m_Downs;

public:

	/**
	 * Add a kill to the playerstate.
	 * This gets called on the server so no need to add a ServerRPC.
	 */
	FORCEINLINE void AddKill()
	{
		m_Kills++;
	}

	/**
	 * Add a death to the playerstate.
	 * This gets called on the server so no need to add a ServerRPC.
	 */
	FORCEINLINE void AddDeath()
	{
		m_Deaths++;
	}

	//void AddDown();

	/** Get the kills of the playerstate. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerState")
	FORCEINLINE int32 GetKills() const { return m_Kills; }

	/** Get the deaths of the playerstate. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerState")
	FORCEINLINE int32 GetDeaths() const { return m_Deaths; }
};
