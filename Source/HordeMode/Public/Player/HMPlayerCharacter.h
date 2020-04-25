// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "Base/HMCharacterBase.h"
#include "HMCommon.h"
#include "HMPlayerCharacter.generated.h"


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

	virtual void BeginPlay() override;
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

	float m_ADSFOV;
	float m_DefaultFOV;

public:

	/** Is the player jumping? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool HasJumped() const { return m_bIsJumping; }

	// 0.1 = diagonal sprinting
	// 0.8 = no diagonal sprinting
	/** Is the player sprinting? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsSprinting() const { return GetCharacterMovement() == nullptr ? false : m_bIsSprinting && !IsADS() && !IsStandingStill() && (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8); }

	/** Is the player standing still? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsStandingStill() const { return GetVelocity().Size() == 0; }

	/** Is the player walking? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsWalking() const { return GetCharacterMovement() == nullptr ? false : !IsSprinting() && !IsStandingStill(); }

	/** Is the player aiming down sights? */
	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FORCEINLINE bool IsADS() const { return m_bIsADS; }

	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	FRotator GetAimOffsets() const;

protected:

	void StartAttack();
	void StopAttack();

	void StartReload();

	void ToggleFireMode();

private:

	/** Get the actor in the characters view */
    UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
    class AActor* GetActorInView();

	UFUNCTION(BlueprintCallable, Category = "HMPlayerCharacter")
	void Interact();

	// TODO: Create a ServerRPC for Interact

    /** The distance that a character can interact with an actor. */
	float m_MaxUseDistance;

	/** The inventory for the character. -- For now we're just going to store the firearms (max 4) */
	UPROPERTY(Replicated)
	TArray<AHMFirearmBase*> m_FirearmInventory;

public:

	bool IsFirearmLocationAvailable(EFirearmAttachLocation Location);

	UPROPERTY(Replicated)
	class AHMFirearmBase* m_CurrentFirearm;

	class AHMFirearmBase* m_PreviousFirearm;

	UFUNCTION(BlueprintPure, Category = "HMPlayerCharacter")
	class AHMFirearmBase* GetCurrentFirearm() const { return m_CurrentFirearm; }

	UPROPERTY(EditDefaultsOnly, Category = "HMPlayerCharacter", meta = (DisplayName = "Start Inventory"))
	TSubclassOf<class AHMFirearmBase> m_StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "HMPlayerCharacter", meta = (DisplayName = "Weapon Attach Socket"))
	FName m_WeaponAttachSocketName;

	void AddFirearm(AHMFirearmBase* NewFirearm);
};