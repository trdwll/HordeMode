// Copyright (c) 2020 Russ 'trdwll' Treadwell

#include "Base/HMCharacterBase.h"
#include "Base/HMGameModeBase.h"
#include "Player/HMPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "HordeMode.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AHMCharacterBase::AHMCharacterBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), m_Health(100.0f), m_MaxHealth(100.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AHMCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMCharacterBase, m_Health);
}

float AHMCharacterBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		m_Health -= ActualDamage;

		if (IsDead())
		{
			Die(ActualDamage, EventInstigator, DamageCauser);
		}
	}

	return ActualDamage;
}

void AHMCharacterBase::Die(float Damage, AController* EventInstigator, AActor* DamageCauser)
{
	// No clue why this was called while the character is alive so lets just return
	if (IsAlive())
	{
		return;
	}

	AHMPlayerState* const KillerPS = Cast<AHMPlayerState>(EventInstigator->PlayerState);
	AHMPlayerState* const VictimPS = Cast<AHMPlayerState>(Controller->PlayerState);
	if (KillerPS && VictimPS)
	{
		if (!AHMPlayerState::IsFriendly(KillerPS, VictimPS))
		{
			GetWorld()->GetAuthGameMode<AHMGameModeBase>()->Killed(EventInstigator, Controller);

			Multi_Ragdoll();

			SetReplicateMovement(false);
			TearOff();

			SetLifeSpan(90.0f);
		}
	}
}

void AHMCharacterBase::Multi_Ragdoll_Implementation()
{
	if (USkeletalMeshComponent* const SkelComp = GetMesh())
	{
		SkelComp->SetAllBodiesSimulatePhysics(true);
		SkelComp->SetSimulatePhysics(true);
		SkelComp->WakeAllRigidBodies();
		SkelComp->bBlendPhysics = true;
		SkelComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	if (UCapsuleComponent* const Comp = GetCapsuleComponent())
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
}

void AHMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}