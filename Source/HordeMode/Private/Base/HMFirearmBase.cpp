// Copyright (c) 2020 Russ 'trdwll' Treadwell

#include "Base/HMFirearmBase.h"
#include "Player/HMPlayerCharacter.h"
#include "Player/HMPlayerController.h"
#include "Player/HMPlayerState.h"
#include "HordeMode.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Curves/CurveVector.h"

AHMFirearmBase::AHMFirearmBase() : m_FirearmID("Default"), m_CurrentFireMode(EFireMode::FullAuto), m_WeaponStatus(EWeaponStatus::Idle)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHMFirearmBase::BeginPlay()
{
	Super::BeginPlay();

	m_FirearmStats = UHMHelpers::GetFirearmStats(GetWorld(), m_FirearmID);
	m_TimeBetweenShots = 60 / m_FirearmStats.ShotsPerMinute;
	m_CurrentAmmo = m_FirearmStats.WeaponInfo.GetDefaultAmmo();
	m_CurrentAmmoInMag = m_FirearmStats.WeaponInfo.MagCapacity;
	m_CurrentFireMode = m_FirearmStats.AllowedFireModes[0];

	PRINT("Firearm Selected : " + m_FirearmStats.WeaponInfo.Title);
}

void AHMFirearmBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFiring() && HasAmmoInMag() && m_FirearmStats.HasRecoil())
	{
		HandleRecoil();
	}
}

void AHMFirearmBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHMFirearmBase, m_HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(AHMFirearmBase, m_WeaponStatus);
}

void AHMFirearmBase::Fire()
{
	if (!HasAmmoInMag() || IsReloading())
	{
		return;
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Fire();
	}

	if (m_CurrentFireMode == EFireMode::ThreeBurst && m_ShotCount == 3)
	{
		StopFire();
		return;
	}

	++m_ShotCount;

	m_WeaponStatus = EWeaponStatus::Firing;
	if (AActor* const MyOwner = GetOwner())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		AHMPlayerCharacter* const Character = Cast<AHMPlayerCharacter>(MyOwner);
		if (Character)
		{
			Character->GetController()->GetPlayerViewPoint(EyeLocation, EyeRotation);
		}
		// MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// TODO: consider shotguns also...
		// TODO: spread

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = m_FirearmStats.WeaponInfo.HitBaseDamage;
			int32 Currency = 25;

			switch (SurfaceType)
			{
			case SURFACE_ZOMBIEVULNERABLE:
				ActualDamage = m_FirearmStats.WeaponInfo.HitHeadshotDamage * 1.5;
				break;
			case SURFACE_ZOMBIEHEAD:
				ActualDamage = m_FirearmStats.WeaponInfo.HitHeadshotDamage;
				Currency = 100;
				break;
			case SURFACE_ZOMBIEBODY:
				ActualDamage = m_FirearmStats.WeaponInfo.HitBodyDamage;
				break;
			case SURFACE_ZOMBIELIMB:
				ActualDamage = m_FirearmStats.WeaponInfo.HitLimbDamage;
				break;
			default:
			case SURFACE_ZOMBIEDEFAULT:
				Currency = 10;
				break;
			}

			if (GetLocalRole() == ROLE_Authority)
			{
				AHMCharacterBase* const HitActor = Cast<AHMCharacterBase>(Hit.GetActor());
				if (HitActor != nullptr && HitActor->IsAlive() && HitActor->GetName().Contains("BP_ZombieCharacter"))
				{
					if (AHMPlayerState* const PS = Cast<AHMPlayerState>(Character->GetPlayerState()))
					{
						PS->AddCurrency(Currency);
					}
				}
			}

			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, nullptr);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

#ifdef _DEBUGDRAW
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
#endif // _DEBUGDRAW

		--m_CurrentAmmoInMag;
		m_OnWeaponAmmoChanged.Broadcast(this, m_CurrentAmmoInMag, m_CurrentAmmo);

		PlayFireEffects(TracerEndPoint);

		if (GetLocalRole() == ROLE_Authority)
		{
			m_HitScanTrace.TraceTo = TracerEndPoint;
			m_HitScanTrace.SurfaceType = SurfaceType;
		}

		m_LastFireTime = GetWorld()->TimeSeconds;
	}

	// Fixes the issue where if in semi auto mode the recoil keeps going
	if (m_CurrentFireMode == EFireMode::SemiAuto && m_ShotCount == 1)
	{
		StopFire();
	}
}

void AHMFirearmBase::OnRep_HitScanTrace()
{
	PlayFireEffects(m_HitScanTrace.TraceTo);
	PlayImpactEffects(m_HitScanTrace.SurfaceType, m_HitScanTrace.TraceTo);
}

bool AHMFirearmBase::Server_Fire_Validate() { return true; }
void AHMFirearmBase::Server_Fire_Implementation() { Fire(); }

bool AHMFirearmBase::Server_Reload_Validate() { return true; }
void AHMFirearmBase::Server_Reload_Implementation() { StartReload(); }

bool AHMFirearmBase::Server_SetStatus_Validate(EWeaponStatus NewStatus) { return true; }
void AHMFirearmBase::Server_SetStatus_Implementation(EWeaponStatus NewStatus) { m_WeaponStatus = NewStatus; }

void AHMFirearmBase::StartFire()
{
	float FirstDelay = FMath::Max(m_LastFireTime + m_TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	m_RecoilTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	switch (m_CurrentFireMode)
	{
	default:
	case EFireMode::ThreeBurst:
	case EFireMode::FullAuto:
		GetWorldTimerManager().SetTimer(m_TimerHandle_TimeBetweenShots, this, &AHMFirearmBase::Fire, m_TimeBetweenShots, true, FirstDelay);
		break;
	case EFireMode::SemiAuto:
		GetWorldTimerManager().SetTimer(m_TimerHandle_TimeBetweenShots, this, &AHMFirearmBase::Fire, m_TimeBetweenShots, false, FirstDelay);
		break;
	}
}

void AHMFirearmBase::StopFire()
{
	if (m_CurrentFireMode == EFireMode::ThreeBurst && m_ShotCount == 3 || m_CurrentFireMode != EFireMode::ThreeBurst)
	{
		if (GetWorldTimerManager().TimerExists(m_TimerHandle_TimeBetweenShots))
		{
			GetWorldTimerManager().ClearTimer(m_TimerHandle_TimeBetweenShots);
		}

		Server_SetStatus(EWeaponStatus::Idle);
		// m_WeaponStatus = EWeaponStatus::Idle;

		m_ShotCount = 0;
		m_RecoilTime = 0.0f;
	}
}

void AHMFirearmBase::StartReload()
{
	if (!CanReload())
	{
		return;
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Reload();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		PlayAnimationMontage(m_FirearmStats.AnimReload.Standing);
	}

	m_WeaponStatus = EWeaponStatus::Reloading;

	m_RecoilTime = 0.0f;

	GetWorldTimerManager().ClearTimer(m_TimerHandle_TimeBetweenShots);

	FTimerHandle TimerHandle_Reload;
	GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &AHMFirearmBase::ReloadFinished, m_FirearmStats.ReloadSpeed);
}

void AHMFirearmBase::ToggleFireMode(EFireMode NewFireMode)
{
	if (m_FirearmStats.AllowedFireModes.Contains(NewFireMode))
	{
		m_OnFirearmFireModeChanged.Broadcast(this, m_CurrentFireMode, NewFireMode);
		m_CurrentFireMode = NewFireMode;
		//Server_SetFireMode(NewFireMode);
	}
}

void AHMFirearmBase::HandleRecoil()
{
	m_RecoilTime += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	if (AHMPlayerCharacter* const Player = Cast<AHMPlayerCharacter>(GetOwner()))
	{
		if (AHMPlayerController* const PlayerController = Cast<AHMPlayerController>(Player->GetController()))
		{
			PlayerController->RegisterRecoil(m_FirearmStats.Recoil->GetVectorValue(m_RecoilTime).Y, m_FirearmStats.Recoil->GetVectorValue(m_RecoilTime).Z);
		}
	}
}

void AHMFirearmBase::ReloadFinished()
{
	// Subtract ammo from m_CurrentAmmo if > 0 and add to m_CurrentAmmoInMag

	int32 ToAdd = FMath::Min(m_CurrentAmmo, m_FirearmStats.WeaponInfo.MagCapacity - m_CurrentAmmoInMag);

	m_CurrentAmmo -= ToAdd;
	m_CurrentAmmoInMag += ToAdd;

	m_OnWeaponAmmoChanged.Broadcast(this, m_CurrentAmmoInMag, m_CurrentAmmo);

	m_WeaponStatus = EWeaponStatus::Idle;

	m_LastFireTime = GetWorld()->TimeSeconds - m_TimeBetweenShots;
}

void AHMFirearmBase::PlayFireEffects(const FVector& TraceEnd)
{
	if (m_FirearmStats.Visuals.MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(m_FirearmStats.Visuals.MuzzleEffect, GetWeaponMesh(), m_FirearmStats.MuzzleSocketName);
	}

	if (m_FirearmStats.Visuals.TracerEffect)
	{
		FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(m_FirearmStats.MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_FirearmStats.Visuals.TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(m_FirearmStats.TracerTargetName, TraceEnd);
		}
	}

	if (m_FirearmStats.Visuals.FireCamShake)
	{
		if (APawn* const MyOwner = Cast<APawn>(GetOwner()))
		{
			if (APlayerController* const PC = Cast<APlayerController>(MyOwner->GetController()))
			{
				PC->ClientPlayCameraShake(m_FirearmStats.Visuals.FireCamShake);
			}
		}
	}
}

void AHMFirearmBase::PlayImpactEffects(EPhysicalSurface SurfaceType, const FVector& ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_ZOMBIEVULNERABLE:
	case SURFACE_ZOMBIEHEAD:
	case SURFACE_ZOMBIEBODY:
	case SURFACE_ZOMBIELIMB:
	case SURFACE_ZOMBIEDEFAULT:
		SelectedEffect = m_FirearmStats.Visuals.FleshImpactEffect;
		break;
	default:
		SelectedEffect = m_FirearmStats.Visuals.DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = GetWeaponMesh()->GetSocketLocation(m_FirearmStats.MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

float AHMFirearmBase::PlayAnimationMontage(UAnimMontage* Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (AHMPlayerCharacter* const MyPawn = Cast<AHMPlayerCharacter>(GetOwner()))
	{
		if (Animation)
		{
			Duration = MyPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
}

void AHMFirearmBase::StopAnimationMontage(UAnimMontage* Animation)
{
	if (AHMPlayerCharacter* const MyPawn = Cast<AHMPlayerCharacter>(GetOwner()))
	{
		if (Animation)
		{
			MyPawn->StopAnimMontage(Animation);
		}
	}
}