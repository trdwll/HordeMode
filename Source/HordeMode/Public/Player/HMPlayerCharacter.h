// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "Base/HMCharacterBase.h"
#include "HMPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCurrencyChangeDelegate, int32, NewCurrency);

/**
 * The class used for players
 */
UCLASS(config=Game)
class HORDEMODE_API AHMPlayerCharacter : public AHMCharacterBase
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName = "Camera Boom"))
	class USpringArmComponent* m_CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName = "Follow Camera"))
	class UCameraComponent* m_FollowCamera;
public:
	AHMPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;


	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (DisplayName = "Base Turn Rate"))
	float m_BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (DisplayName = "Base Lookup Rate"))
	float m_BaseLookUpRate;



protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for crouching input */
	void StartCrouch();
	void StopCrouch();

	/** Called for ads input */
	void StartADS();
	void StopADS();

	/** Called for jump input */
	void StartJump();

	/** Called for sprinting input */
	void StartSprint();
	void StopSprint();


protected:

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return m_CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return m_FollowCamera; }



	/** --- Start HMPlayerCharacter code --- */

private:

	UPROPERTY(Replicated)
	bool m_bIsJumping;

	void SetJumping(bool bJumping);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetJumping(bool bJumping);


	/** Is the player sprinting? */
	UPROPERTY(Replicated)
	bool m_bIsSprinting;

	void SetSprinting(bool bSprint);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetSprinting(bool bSprint);


	/** Is the player ADS? */
	UPROPERTY(Replicated)
	bool m_bIsADS;

	void SetADS(bool bADS);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetADS(bool bADS);

public:

	/** Is the player jumping? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool HasJumped() const { return m_bIsJumping; }

	/** Is the player sprinting? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsSprinting() const
	{
		if (GetCharacterMovement() == nullptr)
		{
			return false;
		}

		// 0.1 = diagonal sprinting
		// 0.8 = no diagonal sprinting
		return m_bIsSprinting && !IsADS() && !IsStandingStill() && (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8);
	}

	/** Is the player standing still? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsStandingStill() const { return GetVelocity().Size() == 0; }

	/** Is the player walking? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsWalking() const
	{
		if (GetCharacterMovement() == nullptr)
		{
			return false;
		}

		return !IsSprinting() && !IsStandingStill();
	}

	/** Is the player aiming down sights? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsADS() const { return m_bIsADS; }

	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FRotator GetAimOffsets() const;
protected:

	void Attack();

private:

	/** Get the actor in the characters view */
    UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
    class AActor* GetActorInView();

	UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
	void Interact();

	// TODO: Create a ServerRPC for Interact

    /** The distance that a character can interact with an actor. */
	float m_MaxUseDistance;

	/** The amount of current a player has. */
	UPROPERTY(Replicated)
	int32 m_Currency;

public:

	/** Reset the players stats. */
	UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
	void ResetPlayer() {}

	/** Get the amount of currency a player has. */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE int32 GetCurrency() const { return m_Currency; }

	/**
	 * Add currency to the player
	 *
	 * @param int32 CurrencyToAdd The amount of currency to add to the player
	 */
	UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
	void AddCurrency(int32 CurrencyToAdd);

	/** --- Server RPC methods --- */
private:

	/**
	 * Server: Add currency to the player
	 *
	 * @param int32 CurrencyToAdd The amount of currency to add to the player
	 */
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AddCurrency(int32 CurrencyToAdd);

protected:

	UPROPERTY(BlueprintAssignable)
	FOnCharacterCurrencyChangeDelegate OnCharacterCurrencyChange;
};

