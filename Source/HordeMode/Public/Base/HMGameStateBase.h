// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HMGameStateBase.generated.h"

/**
 *
 */
UCLASS()
class HORDEMODE_API AHMGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "HMGameStateBase", meta = (DisplayName = "Firearm Stats DataTable"))
    class UDataTable* m_FirearmStatsDataTable;


public:
    UFUNCTION(BlueprintPure, Category = "HMGameStateBase")
    class UDataTable* GetFirearmStatsDataTable() const { return m_FirearmStatsDataTable; }
};
