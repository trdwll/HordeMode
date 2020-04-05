// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EngineUtils.h"

#include "HMCommon.h"
#include "Base/HMGameStateBase.h"

#include "Engine/DataTable.h"

#include "HMHelpers.generated.h"

/**
 * Just a helper class with static methods
 */
UCLASS()
class HORDEMODE_API UHMHelpers final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "HMHelpers")
    static FFirearmStats GetFirearmStats(UWorld* World, const FName& FirearmID)
    {
        if (UDataTable* const Table = Cast<AHMGameStateBase>(World->GetGameState())->GetFirearmStatsDataTable())
        {
            return *Table->FindRow<FFirearmStats>(FirearmID, "");
        }

        return FFirearmStats();
    }
};
