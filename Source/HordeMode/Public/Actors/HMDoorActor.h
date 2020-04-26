// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "Interfaces/Interactable.h"

#include "HMDoorActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorPurchasedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorPayTowardSignature, AHMPlayerState*, SenderPlayerState, int32, CostLeft);

UCLASS()
class HORDEMODE_API AHMDoorActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AHMDoorActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** The cost of the door. */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "HMDoorActor", meta = (DisplayName = "Cost"))
	int32 m_Cost;

	/** Implementation of Interact from IInteractable interface. */
	virtual void Interact_Implementation(class AHMPlayerCharacter* Player) override;


public:

	/** Get the cost of the door. */
	UFUNCTION(BlueprintPure, Category = "HMDoorActor")
	FORCEINLINE int32 GetCost() const { return m_Cost; }

	/*** Delegates */
protected:

	/** Delegate for when the door has been purchased. */
	UPROPERTY(BlueprintAssignable, Category = "HMDoorActor", meta = (DisplayName = "OnDoorPurchased"))
	FOnDoorPurchasedSignature m_OnDoorPurchased;

	/** Delegate for when a player pays towards purchasing the door. */
	UPROPERTY(BlueprintAssignable, Category = "HMDoorActor", meta = (DisplayName = "OnDoorPayToward"))
	FOnDoorPayTowardSignature m_OnDoorPayToward;

};
