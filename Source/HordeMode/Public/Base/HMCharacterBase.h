// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HMCharacterBase.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangedDelegate, AHMCharacterBase*, OwningCharacter, float, Health, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorDeathDelegate);

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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(Replicated)
	float m_Health;

	UPROPERTY(EditDefaultsOnly, Category = "HMCharacterBase", meta = (DisplayName = "Max Health"))
	float m_MaxHealth;

public:
	/** Get the characters health */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE float GetHealth() const { return m_Health; }

	/** Get the default health of the character. */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE float GetDefaultHealth() const { return GetClass()->GetDefaultObject<AHMCharacterBase>()->m_Health; }

	/** Get the max health */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE float GetMaxHealth() const { return m_MaxHealth; }

	/** Get Is the character alive? */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE bool IsAlive() const { return m_Health > 0.0f; }

	/** Get Is the character dead? */
	UFUNCTION(BlueprintPure, Category = "HMCharacterBase")
	FORCEINLINE bool IsDead() const { return m_Health <= 0.0f; }


};
