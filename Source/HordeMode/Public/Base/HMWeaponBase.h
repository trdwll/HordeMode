// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HMCommon.h"

#include "HMWeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAmmoChangedSignature, AHMWeaponBase*, WeaponActor, int32, CurrentAmmoInMag, int32, CurrentAmmo);

/**
 * A base class for all weapons - firearms, grenade launchers/rocket launchers, melee, and even crossbow/bow etc
 */
UCLASS(Abstract, Blueprintable)
class HORDEMODE_API AHMWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AHMWeaponBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, Category = "HMWeaponBase", meta = (DisplayName = "Weapon Mesh"))
	class USkeletalMeshComponent* m_WeaponMesh;

private:

	EWeaponAttachLocation m_CurrentAttachLocation;

protected:

	/** The current ammo in the reserve minus the current mag. */
	UPROPERTY(Replicated)
	int32 m_CurrentAmmo;

	/** The current amount of ammo that this weapon has in it's mag. NOTE: Sure a bow etc doesn't have a magazine, but it still has a "round in the chamber". */
	UPROPERTY(Replicated)
	int32 m_CurrentAmmoInMag;

public:

	virtual void StartFire() PURE_VIRTUAL(StartFire, );
	virtual void StopFire() PURE_VIRTUAL(StopFire, );
	virtual void StartReload() PURE_VIRTUAL(StartReload, );

public:

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE class USkeletalMeshComponent* GetWeaponMesh() const { return m_WeaponMesh; }

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE EWeaponAttachLocation GetAttachLocation() const { return m_CurrentAttachLocation; }

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE int32 GetCurrentAmmoInMag() const { return m_CurrentAmmoInMag; }

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE int32 GetCurrentAmmo() const { return m_CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE bool HasAmmoInMag() const { return m_CurrentAmmoInMag > 0; }

	UFUNCTION(BlueprintPure, Category = "HMWeaponBase")
	FORCEINLINE bool HasAmmo() const { return m_CurrentAmmoInMag > 0 && m_CurrentAmmo > 0; }

protected:

	UPROPERTY(BlueprintAssignable, Category = "HMWeaponBase", meta = (DisplayName = "OnWeaponAmmoChanged"))
	FOnWeaponAmmoChangedSignature m_OnWeaponAmmoChanged;

};
