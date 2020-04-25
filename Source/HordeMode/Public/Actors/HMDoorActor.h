// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "Interfaces/Interactable.h"

#include "HMDoorActor.generated.h"


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

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "HMDoorActor", meta = (DisplayName = "Cost"))
	int32 m_Cost;

	virtual void Interact_Implementation(class AHMPlayerCharacter* Player) override;

};
