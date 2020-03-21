// Copyright (c) 2020 Russ 'trdwll' Treadwell


#include "Components/HMCharacterMovementComponent.h"
#include "Player/HMPlayerCharacter.h"

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
