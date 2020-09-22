// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "Base/HMWeaponBase.h"

#include "HMFirearmBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFirearmFireModeChangedSignature, AHMFirearmBase*, FirearmActor, EFireMode, CurrentFireMode, EFireMode, NewFireMode);


/**
 * This is the base class for all types of firearms. - Rifle, pistol, rpg
 */
UCLASS()
class HORDEMODE_API AHMFirearmBase : public AHMWeaponBase
{
	GENERATED_BODY()

public:
	AHMFirearmBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	/** --- HMFirearmBase code --- */
protected:

	UPROPERTY(EditDefaultsOnly, Category = "HMFirearmBase", meta = (DisplayName = "Firearm ID"))
	FName m_FirearmID;

private:

	/** Just an internal counter for the 3 round burst firemode. */
	uint8 m_ShotCount;

	/** Store the default firearm stats. */
	FFirearmStats m_FirearmStats;

	EFireMode m_CurrentFireMode;

	UPROPERTY(Replicated)
	EWeaponStatus m_WeaponStatus;

	float m_RecoilTime;

	FTimerHandle m_TimerHandle_TimeBetweenShots;

	float m_LastFireTime;
	float m_TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace m_HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	void PlayFireEffects(const FVector& TraceEnd);
	void PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector& ImpactPoint);

	float PlayAnimationMontage(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	void StopAnimationMontage(UAnimMontage* Animation);

	/** Server RPCs */
	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire();

	void ReloadFinished();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reload();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetStatus(EWeaponStatus NewStatus);

	void HandleRecoil();

public:

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE FFirearmStats GetFirearmStats() const { return m_FirearmStats; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE EFireMode GetFireMode() const { return m_CurrentFireMode; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool IsReloading() const { return m_WeaponStatus == EWeaponStatus::Reloading; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool CanReload() const { return m_CurrentAmmo > 0 && m_CurrentAmmoInMag < m_FirearmStats.WeaponInfo.MagCapacity && !IsReloading(); }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool IsFiring() const { return m_WeaponStatus == EWeaponStatus::Firing && m_CurrentAmmoInMag > 0; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE FString GetFireModeAsString(EFireMode FireMode) { return m_FirearmStats.ConvertFireModeToString(FireMode); }

	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void StartReload() override;

	void ToggleFireMode(EFireMode NewFireMode);

	void Unjam() {}

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FString GetStatus() const
	{
		switch (m_WeaponStatus)
		{
		case EWeaponStatus::Equipping: return "Equipping";
		case EWeaponStatus::Firing: return "Firing";
		case EWeaponStatus::Idle: return "Idle";
		case EWeaponStatus::Jammed: return "Jammed";
		case EWeaponStatus::Reloading: return "Reloading";
		}

		return "Invalid status";
	}

	/** Delegates */
protected:

	UPROPERTY(BlueprintAssignable, Category = "HMFirearmBase", meta = (DisplayName = "OnFirearmFireModeChanged"))
	FOnFirearmFireModeChangedSignature m_OnFirearmFireModeChanged;
};
