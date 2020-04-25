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

AHMFirearmBase::AHMFirearmBase() : m_FirearmID("Default"), m_CurrentAttachLocation(EFirearmAttachLocation::Hands), m_CurrentFireMode(EFireMode::FullAuto), m_FirearmStatus(EFirearmStatus::Idle),
m_CurrentAmmo(0), m_CurrentAmmoInMag(0)
{
	PrimaryActorTick.bCanEverTick = true;

	m_FirearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirearmMesh"));
	RootComponent = m_FirearmMesh;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void AHMFirearmBase::BeginPlay()
{
	Super::BeginPlay();

	m_FirearmStats = UHMHelpers::GetFirearmStats(GetWorld(), m_FirearmID);
	m_TimeBetweenShots = 60 / m_FirearmStats.ShotsPerMinute;
	m_CurrentAmmo = m_FirearmStats.GetDefaultAmmo();
	m_CurrentAmmoInMag = m_FirearmStats.MagCapacity;
	m_CurrentFireMode = m_FirearmStats.AllowedFireModes[0];

	m_HorizontalKick = m_FirearmStats.GetHRecoil();
	m_VerticalKick = m_FirearmStats.GetVRecoil();

	m_TargetHorizontalRecoil = m_HorizontalKick;
	m_TargetVerticalRecoil = m_VerticalKick;

#ifdef _DEBUG
	if (m_bUnlimitedAmmo)
	{
		m_CurrentAmmo = 9999;
		m_CurrentAmmoInMag = 9999;
	}
#endif // _DEBUG

	PRINT("Firearm Selected : " + m_FirearmStats.Title + " : " + FString::SanitizeFloat(m_TimeBetweenShots));
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
	DOREPLIFETIME_CONDITION(AHMFirearmBase, m_CurrentAmmo, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHMFirearmBase, m_CurrentAmmoInMag, COND_SkipOwner);
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

	m_FirearmStatus = EFirearmStatus::Firing;
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

			float ActualDamage = m_FirearmStats.HitBaseDamage;
			int32 Currency = 25;

			switch (SurfaceType)
			{
			case SURFACE_ZOMBIEVULNERABLE:
				ActualDamage = m_FirearmStats.HitHeadshotDamage * 1.5;
				break;
			case SURFACE_ZOMBIEHEAD:
				ActualDamage = m_FirearmStats.HitHeadshotDamage;
				Currency = 100;
				break;
			case SURFACE_ZOMBIEBODY:
				ActualDamage = m_FirearmStats.HitBodyDamage;
				break;
			case SURFACE_ZOMBIELIMB:
				ActualDamage = m_FirearmStats.HitLimbDamage;
				break;
			default:
			case SURFACE_ZOMBIEDEFAULT:
				ActualDamage = m_FirearmStats.HitBaseDamage;
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
		m_OnFirearmAmmoChanged.Broadcast(this, m_CurrentAmmoInMag, m_CurrentAmmo);

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
void AHMFirearmBase::Server_Reload_Implementation() { Reload(); }

void AHMFirearmBase::StartFire()
{
	float FirstDelay = FMath::Max(m_LastFireTime + m_TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

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

		m_FirearmStatus = EFirearmStatus::Idle;
		m_ShotCount = 0;
	}
}

void AHMFirearmBase::Reload()
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
	m_FirearmStatus = EFirearmStatus::Reloading;

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
	}
}

void AHMFirearmBase::HandleRecoil()
{
	float Horizontal = UKismetMathLibrary::FInterpTo(m_CurrentHorizontalRecoil, m_HorizontalKick, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 20.0f);
	float Vertical = UKismetMathLibrary::FInterpTo(m_CurrentVerticalRecoil, m_VerticalKick, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 20.0f);

	if (Horizontal != 0.0f || Vertical != 0.0f)
	{
		if (AHMPlayerCharacter* const Player = Cast<AHMPlayerCharacter>(GetOwner()))
		{
			if (AHMPlayerController* const PlayerController = Cast<AHMPlayerController>(Player->GetController()))
			{
				PlayerController->RegisterRecoil(Horizontal, Vertical);
			}
		}
	}
}

void AHMFirearmBase::ReloadFinished()
{
	// Subtract ammo from m_CurrentAmmo if > 0 and add to m_CurrentAmmoInMag

	int32 ToAdd = FMath::Min(m_CurrentAmmo, m_FirearmStats.MagCapacity - m_CurrentAmmoInMag);

	m_CurrentAmmo -= ToAdd;
	m_CurrentAmmoInMag += ToAdd;

	m_OnFirearmAmmoChanged.Broadcast(this, m_CurrentAmmoInMag, m_CurrentAmmo);

	m_FirearmStatus = EFirearmStatus::Idle;

	m_LastFireTime = GetWorld()->TimeSeconds - m_TimeBetweenShots;
}

void AHMFirearmBase::PlayFireEffects(const FVector& TraceEnd)
{
	if (m_FirearmStats.FirearmVisuals.MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(m_FirearmStats.FirearmVisuals.MuzzleEffect, m_FirearmMesh, m_FirearmStats.MuzzleSocketName);
	}

	if (m_FirearmStats.FirearmVisuals.TracerEffect)
	{
		FVector MuzzleLocation = m_FirearmMesh->GetSocketLocation(m_FirearmStats.MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_FirearmStats.FirearmVisuals.TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(m_FirearmStats.TracerTargetName, TraceEnd);
		}
	}

	if (m_FirearmStats.FirearmVisuals.FireCamShake)
	{
		if (APawn* const MyOwner = Cast<APawn>(GetOwner()))
		{
			if (APlayerController* const PC = Cast<APlayerController>(MyOwner->GetController()))
			{
				PC->ClientPlayCameraShake(m_FirearmStats.FirearmVisuals.FireCamShake);
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
		SelectedEffect = m_FirearmStats.FirearmVisuals.FleshImpactEffect;
		break;
	default:
		SelectedEffect = m_FirearmStats.FirearmVisuals.DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = m_FirearmMesh->GetSocketLocation(m_FirearmStats.MuzzleSocketName);

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