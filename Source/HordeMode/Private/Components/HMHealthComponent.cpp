// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Components/HMHealthComponent.h"
#include "Net/UnrealNetwork.h"

UHMHealthComponent::UHMHealthComponent()
	: m_Team(ETeamType::ETT_NONE), m_DefaultHealth(100.0f), m_Health(m_DefaultHealth)
{
	SetIsReplicated(true);
}


void UHMHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (AActor* MyOwner = GetOwner())
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHMHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void UHMHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHMHealthComponent, m_Health);
}

void UHMHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0.0f || DamageCauser != DamagedActor && IsFriend(DamagedActor, DamageCauser))
	{
		return;
	}

	m_Health = FMath::Clamp(m_Health - Damage, 0.0f, m_DefaultHealth);

	OnHealthChanged.Broadcast(this, m_Health, DamageType, InstigatedBy, DamageCauser);

	if (!IsAlive())
	{
		// TODO: cast to GM and run Killed

		OnActorDeath.Broadcast();
	}
}

void UHMHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = m_Health - OldHealth;

	OnHealthChanged.Broadcast(this, m_Health, nullptr, nullptr, nullptr);
}

bool UHMHealthComponent::IsFriend(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	UHMHealthComponent* HealthCompA = Cast<UHMHealthComponent>(ActorA->GetComponentByClass(UHMHealthComponent::StaticClass()));
	UHMHealthComponent* HealthCompB = Cast<UHMHealthComponent>(ActorB->GetComponentByClass(UHMHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	return HealthCompA->GetTeam() == HealthCompB->GetTeam();
}

void UHMHealthComponent::Heal(float Amount)
{
	// Should we not allow healing dead actors?
	if (Amount <= 0.0f || m_Health <= 0.0f)
	{
		return;
	}

	m_Health = FMath::Clamp(m_Health + Amount, 0.0f, m_DefaultHealth);

	OnHealthChanged.Broadcast(this, m_Health, nullptr, nullptr, nullptr);
}

