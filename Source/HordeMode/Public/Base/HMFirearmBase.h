// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HMCommon.h"

#include "HMFirearmBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFirearmAmmoChangedSignature, AHMFirearmBase*, FirearmActor, int32, CurrentAmmoInMag, int32, CurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFirearmFireModeChangedSignature, AHMFirearmBase*, FirearmActor, EFireMode, CurrentFireMode, EFireMode, NewFireMode);

UCLASS()
class HORDEMODE_API AHMFirearmBase : public AActor
{
	GENERATED_BODY()

public:
	AHMFirearmBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	/** --- HMFirearmBase code --- */
protected:

	UPROPERTY(VisibleAnywhere, Category = "HMFirearmBase", meta = (DisplayName = "Firearm Mesh"))
	class USkeletalMeshComponent* m_FirearmMesh;

	UPROPERTY(EditDefaultsOnly, Category = "HMFirearmBase", meta = (DisplayName = "Firearm ID"))
	FName m_FirearmID;

private:

	/** Just an internal counter for the 3 round burst firemode. */
	uint8 m_ShotCount;

	/** Store the default firearm stats. */
	FFirearmStats m_FirearmStats;

	EFirearmAttachLocation m_CurrentAttachLocation;

	UPROPERTY(Replicated)
	EFireMode m_CurrentFireMode;

	EFirearmStatus m_FirearmStatus;

	UPROPERTY(Replicated)
	int32 m_CurrentAmmo;

	UPROPERTY(Replicated)
	int32 m_CurrentAmmoInMag;

	float m_CurrentHorizontalRecoil;
	float m_CurrentVerticalRecoil;

	float m_TargetHorizontalRecoil;
	float m_TargetVerticalRecoil;

	/** The time that it takes to reset the recoil to 0. */
	float m_TimeToResetRecoil;

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

	void HandleRecoil();

public:

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE FFirearmStats GetFirearmStats() const { return m_FirearmStats; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE class USkeletalMeshComponent* GetFirearmMesh() const { return m_FirearmMesh; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE EFirearmAttachLocation GetAttachLocation() const { return m_CurrentAttachLocation; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE EFireMode GetFireMode() const { return m_CurrentFireMode; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool IsReloading() const { return m_FirearmStatus == EFirearmStatus::Reloading; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE int32 GetCurrentAmmoInMag() const { return m_CurrentAmmoInMag; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE int32 GetCurrentAmmo() const { return m_CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool HasAmmoInMag() const { return m_CurrentAmmoInMag > 0; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool HasAmmo() const { return m_CurrentAmmoInMag > 0 && m_CurrentAmmo > 0; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool CanReload() const { return m_CurrentAmmo > 0 && m_CurrentAmmoInMag < m_FirearmStats.MagCapacity && !IsReloading(); }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE bool IsFiring() const { return m_FirearmStatus == EFirearmStatus::Firing; }

	UFUNCTION(BlueprintPure, Category = "HMFirearmBase")
	FORCEINLINE FString GetFireModeAsString(EFireMode FireMode) { return m_FirearmStats.ConvertFireModeToString(FireMode); }

	void StartFire();
	void StopFire();

	void Reload();
	void ToggleFireMode(EFireMode NewFireMode);

	void Unjam() {}

	/** Developer stuff */
protected:
	const bool m_bUnlimitedAmmo = false;

	/** Delegates */
protected:

	UPROPERTY(BlueprintAssignable, Category = "HMFirearmBase", meta = (DisplayName = "OnFirearmAmmoChanged"))
	FOnFirearmAmmoChangedSignature m_OnFirearmAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "HMFirearmBase", meta = (DisplayName = "OnFirearmFireModeChanged"))
	FOnFirearmFireModeChangedSignature m_OnFirearmFireModeChanged;
};
