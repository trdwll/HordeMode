// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HMHealthComponent.generated.h"

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	ETT_NONE	UMETA(DisplayName = "None"),
	ETT_PLAYER	UMETA(DisplayName = "Player"),
	ETT_AI		UMETA(DisplayName = "AI")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangedDelegate, UHMHealthComponent*, OwningHealthComp, float, Health, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorDeathDelegate);

UCLASS( ClassGroup=(HordeMode), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API UHMHealthComponent final : public UActorComponent
{
	GENERATED_BODY()


public:
	UHMHealthComponent();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	/** --- Start Health Component code --- */
protected:

	/**
	 * Delegate for when the actor takes damage
	 *
	 * @param AActor* DamagedActor {DESCRIPTION}
	 * @param float Damage {DESCRIPTION}
	 * @param const class UDamageType* DamageType {DESCRIPTION}
	 * @param class AController* InstigatedBy {DESCRIPTION}
	 * @param AActor* DamageCauser {DESCRIPTION}
	 */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** The team that this actor is. */
	UPROPERTY(EditDefaultsOnly, Category = "HMHealthComponent", meta = (DisplayName = "Team"))
	ETeamType m_Team;

	/** The default health value for the actor. */
	UPROPERTY(EditDefaultsOnly, Category = "HMHealthComponent", meta = (DisplayName = "Default Health"))
	float m_DefaultHealth;

private:
	/** The current health value for this actor. */
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float m_Health;

	/** Replication method for the variable m_Health */
	UFUNCTION()
	void OnRep_Health(float OldHealth);

public:

	/** Get the team of the actor. */
	UFUNCTION(BlueprintPure, Category = "HMHealthComponent")
	FORCEINLINE ETeamType GetTeam() const { return m_Team; }

	/** Get the current health of the actor. */
	UFUNCTION(BlueprintPure, Category = "HMHealthComponent")
	FORCEINLINE float GetHealth() const { return m_Health; }

	/** Is this actor still alive? */
	UFUNCTION(BlueprintPure, Category = "HMHealthComponent")
	FORCEINLINE bool IsAlive() const { return m_Health > 0.0f; }


	/**
	 * Are the two actors on the same team?
	 *
	 * @param AActor* ActorA
	 * @param AActor* ActorB
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HMHealthComponent")
	static bool IsFriend(AActor* ActorA, AActor* ActorB);

	/**
	 * Heal the actor
	 *
	 * @param float Amount The amount of health that will be applied to the actor.
	 */
	UFUNCTION(BlueprintCallable, Category = "HMHealthComponent")
	void Heal(float Amount);


	/** Delegate for when the health is changed for this actor. */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedDelegate OnHealthChanged;

	/** Delegate for when the actor is killed. */
	UPROPERTY(BlueprintAssignable)
	FOnActorDeathDelegate OnActorDeath;
};
