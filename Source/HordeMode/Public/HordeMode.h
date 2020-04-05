// Copyright (c) 2020 Russ 'trdwll' Treadwell

#pragma once

#include "CoreMinimal.h"
#include "HMHelpers.h"

#define _DEBUG
// #define _DEBUGDRAW

#ifdef _DEBUG
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

#define PRINT(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, CUR_CLASS_LINE + ": " + text)
#define LOG(str, ...) UE_LOG(LogTemp, Log, TEXT("%s: %s"), *CUR_CLASS, *FString::Printf(TEXT(str), ##__VA_ARGS__))

/** Current Class Name + Function Name where this is called! */
#define CUR_CLASS_FUNC (FString(__FUNCTION__))

/** Current Class where this is called! */
#define CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

/** Current Function Name where this is called! */
#define CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

/** Current Line Number in the code where this is called! */
#define CUR_LINE  (FString::FromInt(__LINE__))

/** Current Class and Line Number where this is called! */
#define CUR_CLASS_LINE (CUR_CLASS + "(" + CUR_LINE + ")")

/** Current Function Signature where this is called! */
#define CUR_FUNCSIG (FString(__FUNCSIG__))
#else
#define PRINT(text)
#define LOG(str, ...)
#endif // _DEBUG


#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Concrete	SurfaceType1
#define SURFACE_Dirt		SurfaceType2
#define SURFACE_Water		SurfaceType3
#define SURFACE_Metal		SurfaceType4
#define SURFACE_Wood		SurfaceType5
#define SURFACE_Grass		SurfaceType6
#define SURFACE_Glass		SurfaceType7
#define SURFACE_Flesh		SurfaceType8

#define SURFACE_ZOMBIEHEAD			SurfaceType9
#define SURFACE_ZOMBIEBODY			SurfaceType10
#define SURFACE_ZOMBIELIMB			SurfaceType11
#define SURFACE_ZOMBIEDEFAULT		SurfaceType12
#define SURFACE_ZOMBIEVULNERABLE	SurfaceType13

#define COLLISION_WEAPON			ECC_GameTraceChannel1

