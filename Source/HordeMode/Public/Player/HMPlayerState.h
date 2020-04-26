// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"

#include "HMCommon.h"

#include "GameFramework/PlayerState.h"
#include "HMPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCurrencyChangeDelegate, int32, NewCurrency);

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

	UPROPERTY(Replicated)
	ETeamType m_TeamType;

	UPROPERTY(Replicated)
	int32 m_Currency;

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

	FORCEINLINE void ChangeTeamType(ETeamType NewTeamType) { m_TeamType = NewTeamType; }
	FORCEINLINE ETeamType GetTeamType() const { return m_TeamType; }

	static bool IsFriendly(AHMPlayerState* A, AHMPlayerState* B) { return A == nullptr || B == nullptr || A->GetTeamType() == B->GetTeamType(); }

	/**
	 * Add a kill to the playerstate.
	 * This gets called on the server so no need to add a ServerRPC.
	 */
	FORCEINLINE void AddKill() { m_Kills++; }

	/**
	 * Add a death to the playerstate.
	 * This gets called on the server so no need to add a ServerRPC.
	 */
	FORCEINLINE void AddDeath() { m_Deaths++; }

	//void AddDown();

	/** Get the amount of currency a player has. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerState")
	FORCEINLINE int32 GetCurrency() const { return m_Currency; }

	/** Get the kills of the playerstate. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerState")
	FORCEINLINE int32 GetKills() const { return m_Kills; }

	/** Get the deaths of the playerstate. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerState")
	FORCEINLINE int32 GetDeaths() const { return m_Deaths; }

	/**
	 * Add currency to the player
	 *
	 * @param int32 CurrencyToAdd The amount of currency to add to the player
	 */
	UFUNCTION(BlueprintCallable, Category = "HMPlayerState")
	void AddCurrency(int32 CurrencyToAdd);

	void SubtractCurrency(int32 CurrencyToSubtract);

	UFUNCTION(BlueprintCallable, Category = "HMPlayerState")
	void SetCurrency(int32 NewCurrency);

	/** --- Server RPC methods --- */
private:

	/**
	 * Server: Add currency to the player
	 *
	 * @param int32 CurrencyToAdd The amount of currency to add to the player
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AddCurrency(int32 CurrencyToAdd);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SubtractCurrency(int32 CurrencyToSubtract);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetCurrency(int32 NewCurrency);

protected:

	UPROPERTY(BlueprintAssignable)
	FOnCharacterCurrencyChangeDelegate OnCharacterCurrencyChange;
};
