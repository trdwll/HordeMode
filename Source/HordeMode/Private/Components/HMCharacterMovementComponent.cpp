// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Components/HMCharacterMovementComponent.h"
#include "Player/HMPlayerCharacter.h"

UHMCharacterMovementComponent::UHMCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseControllerDesiredRotation = true;
	bOrientRotationToMovement = false;
}

float UHMCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	if (AHMPlayerCharacter* const Owner = Cast<AHMPlayerCharacter>(PawnOwner))
	{
		if (IsCrouching() || Owner->IsADS())
		{
			MaxSpeed *= 0.5f;
		}
		else if (Owner->IsSprinting())
		{
			MaxSpeed *= 1.5f;
		}
	}

	return MaxSpeed;
}
