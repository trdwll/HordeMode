// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once
#include "Engine/DataTable.h"
#include "HMCommon.generated.h"

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UENUM()
enum class EWeaponAttachLocation : uint8
{
	Hands			UMETA(DisplayName = "Hands (Currently Equipped)"),
	Right_Back		UMETA(DisplayName = "Right Back"),
	Left_Back		UMETA(DisplayName = "Left Back"),
	Right_Hip		UMETA(DisplayName = "Right Hip"),
	Left_Hip		UMETA(DisplayName = "Left Hip")
};

UENUM()
enum class EFireMode : uint8
{
	FullAuto		UMETA(DisplayName = "Fully Automatic"),
	ThreeBurst		UMETA(DisplayName = "Three Round Burst"),
	SemiAuto		UMETA(DisplayName = "Semi Automatic")
};

UENUM()
enum class EWeaponStatus : uint8
{
	Idle			UMETA(DisplayName = "Idle (equipped or 'holstered')"),
	Firing			UMETA(DisplayName = "Firing"),
	Reloading		UMETA(DisplayName = "Reloading"),
	Equipping		UMETA(DisplayName = "Equipping"),
	Jammed			UMETA(DisplayName = "Jammed")
};

UENUM()
enum class EFirearmType : uint8
{
	Pistol		UMETA(DisplayName = "Pistol"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Shotgun		UMETA(DisplayName = "Shotgun"),
	Other		UMETA(DisplayName = "Other") // For rocket launchers etc
};


/** Effects */
USTRUCT(BlueprintType)
struct FWeaponVisuals
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCameraShake> FireCamShake;

	FWeaponVisuals() :
		MuzzleEffect(nullptr), DefaultImpactEffect(nullptr), FleshImpactEffect(nullptr), TracerEffect(nullptr), FireCamShake(nullptr)
	{}
};

/** Sounds */
USTRUCT(BlueprintType)
struct FWeaponSounds
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Fire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Reload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Jammed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Unjammed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Equip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* Unequipped;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* ToggleFireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* OutOfAmmo;

	FWeaponSounds() :
		Fire(nullptr), Reload(nullptr), Jammed(nullptr), Unjammed(nullptr), Equip(nullptr), Unequipped(nullptr), ToggleFireMode(nullptr), OutOfAmmo(nullptr)
	{}
};

/** Animations */
USTRUCT(BlueprintType)
struct FWeaponAnims
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Standing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Crouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Prone;

	FWeaponAnims() :
		Standing(nullptr), Crouching(nullptr), Prone(nullptr)
	{}
};

// TODO: Add data for projectiles
USTRUCT(BlueprintType)
struct FProjectileData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Description;

	// TODO: Expand this later to support multiple locations (so a primary can be on the right or left back etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponAttachLocation AttachLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MagCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MagCount;

	/// Damage

	/** Headshot damage amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitHeadshotDamage;

	/** Body damage amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitBodyDamage;

	/** Limb damage amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitLimbDamage;

	/** Default damage amount */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitBaseDamage;

	FWeaponInfo() :
		Title("N/A"), Description("No Description"), AttachLocation(EWeaponAttachLocation::Right_Back), MagCapacity(30), MagCount(8),
		HitHeadshotDamage(80.0f), HitBodyDamage(40.0f), HitLimbDamage(20.0f), HitBaseDamage(30.0f)
	{}

	int32 GetDefaultAmmo() const { return MagCount * MagCapacity; }
	int32 GetDefaultAmmo(bool bSubtractInitialMag) const { return bSubtractInitialMag ? GetDefaultAmmo() - MagCapacity : GetDefaultAmmo(); }
};

USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponInfo WeaponInfo;

	// TODO: Add additional info that's required by all weapon types

	FWeaponStats() {}
};

USTRUCT(BlueprintType)
struct FFirearmStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponInfo WeaponInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EFireMode> AllowedFireModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShotsPerMinute;

	/** How many bullets should be shot at a time? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 ShotCount;

	/** What kind of firearm is this? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFirearmType FirearmType;

	/// Accuracy

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D HorizontalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D VerticalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D HorizontalSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D VerticalSpread;

	///

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FName TracerTargetName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FWeaponVisuals Visuals;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FWeaponSounds Sounds;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FWeaponAnims AnimReload;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FWeaponAnims AnimJammed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FWeaponAnims AnimFireMode;

	FFirearmStats() :
		ShotsPerMinute(600), ShotCount(1),
		ReloadSpeed(3.0f),
		MuzzleSocketName("MuzzleFlashSocket"), TracerTargetName("Target")
	{}

	FString ConvertFireModeToString(EFireMode ToConvert)
	{
		if (ToConvert == EFireMode::FullAuto) return "Fully Automatic";
		if (ToConvert == EFireMode::SemiAuto) return "Semi Automatic";
		if (ToConvert == EFireMode::ThreeBurst) return "3 Round Burst";

		return "N/A";
	}

	float GetHRecoil() const { return FMath::FRandRange(HorizontalRecoil.X, HorizontalRecoil.Y); }
	float GetVRecoil() const { return FMath::FRandRange(VerticalRecoil.X, VerticalRecoil.Y) * -1.0f; }
	float GetHRecoil(float Min, float Max) const { return FMath::FRandRange(Min, Max); }
	float GetVRecoil(float Min, float Max) const { return FMath::FRandRange(Min, Max) * -1.0f; }

	bool HasRecoil() const { return GetHRecoil() != 0.0f || GetVRecoil() != 0.0f; }
	bool HasProjectile() const { return ProjectileClass == nullptr; }
};


 UENUM(BlueprintType)
 enum class ETeamType : uint8
 {
 	Player		UMETA(DisplayName = "Player"),
 	Spectator	UMETA(DisplayName = "Spectator"),
 	Enemy		UMETA(DisplayName = "Enemy")
 };