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
enum class EFirearmAttachLocation : uint8
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
enum class EFirearmStatus : uint8
{
	Idle			UMETA(DisplayName = "Idle (equipped or 'holstered')"),
	Firing			UMETA(DisplayName = "Firing"),
	Reloading		UMETA(DisplayName = "Reloading"),
	Equipping		UMETA(DisplayName = "Equipping"),
	Jammed			UMETA(DisplayName = "Jammed")
};


/** Effects */
USTRUCT(BlueprintType)
struct FFirearmVisuals
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

	FFirearmVisuals() :
		MuzzleEffect(nullptr), DefaultImpactEffect(nullptr), FleshImpactEffect(nullptr), TracerEffect(nullptr), FireCamShake(nullptr)
	{}
};

/** Sounds */
USTRUCT(BlueprintType)
struct FFirearmSounds
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

	FFirearmSounds() :
		Fire(nullptr), Reload(nullptr), Jammed(nullptr), Unjammed(nullptr), Equip(nullptr), Unequipped(nullptr), ToggleFireMode(nullptr), OutOfAmmo(nullptr)
	{}
};

/** Animations */
USTRUCT(BlueprintType)
struct FFirearmAnims
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Standing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Crouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* Prone;

	FFirearmAnims() :
		Standing(nullptr), Crouching(nullptr), Prone(nullptr)
	{}
};

USTRUCT(BlueprintType)
struct FFirearmStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Description;

	// TODO: Expand this later to support multiple locations (so a primary can be on the right or left back etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFirearmAttachLocation AttachLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EFireMode> AllowedFireModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MagCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MagCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShotsPerMinute;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D HorizontalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D VerticalRecoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D HorizontalSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D VerticalSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bHasProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FName TracerTargetName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FFirearmVisuals FirearmVisuals;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FFirearmSounds FirearmSounds;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FFirearmAnims AnimReload;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FFirearmAnims AnimJammed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FFirearmAnims AnimFireMode;

	FFirearmStats() :
		Title("NoTitle"), Description("NoDescription"), AttachLocation(EFirearmAttachLocation::Right_Back), MagCapacity(30), MagCount(8), ShotsPerMinute(600),
		HitHeadshotDamage(80.0f), HitBodyDamage(40.0f), HitLimbDamage(20.0f), HitBaseDamage(30.0f),
		ReloadSpeed(3.0f),
		MuzzleSocketName("MuzzleFlashSocket"), TracerTargetName("Target")
	{}

	int32 GetDefaultAmmo() const { return MagCount * MagCapacity; }
	int32 GetDefaultAmmo(bool bSubtractInitialMag) const { return bSubtractInitialMag ? GetDefaultAmmo() - MagCapacity : GetDefaultAmmo(); }
	FString ConvertFireModeToString(EFireMode ToConvert)
	{
		if (ToConvert == EFireMode::FullAuto) return "Fully Automatic";
		if (ToConvert == EFireMode::SemiAuto) return "Semi Automatic";
		if (ToConvert == EFireMode::ThreeBurst) return "3 Round Burst";

		return "N/A";
	}
	float CalculateHRecoil() { return FMath::FRandRange(HorizontalRecoil.X, HorizontalRecoil.Y); }
	float CalculateVRecoil() { return FMath::FRandRange(VerticalRecoil.X, VerticalRecoil.Y) * -1.0f; }
	float CalculateHRecoil(float Min, float Max) { return FMath::FRandRange(Min, Max); }
	float CalculateVRecoil(float Min, float Max) { return FMath::FRandRange(Min, Max) * -1.0f; }
};
