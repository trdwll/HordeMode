// Copyright (c) 2020 Russ 'trdwll' Treadwell

#include "Player/HMPlayerCharacter.h"

#include "Net/UnrealNetwork.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/HMCharacterMovementComponent.h"
#include "Interfaces/Interactable.h"
#include "Base/HMWeaponBase.h"
#include "Base/HMFirearmBase.h"
#include "HordeMode.h"

AHMPlayerCharacter::AHMPlayerCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)),
	m_BaseTurnRate(45.0f), m_BaseLookUpRate(45.0f), m_bIsSprinting(false), m_ADSFOV(65.0f), m_DefaultFOV(90.0f), m_MaxUseDistance(380.0f), m_WeaponAttachSocketName("WeaponSocket")
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; // If we set to true then the second player will be jittery when walking
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(RootComponent);
	m_CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	m_CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	m_CameraBoom->SocketOffset = FVector(0, 35, 0);
	m_CameraBoom->TargetOffset = FVector(0, 0, 55);

	// Create a follow camera
	m_FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	m_FollowCamera->SetupAttachment(m_CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	m_FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AHMPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_DefaultFOV = m_FollowCamera->FieldOfView;

	if (GetLocalRole() == ROLE_Authority)
	{
		// Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		m_CurrentWeapon = GetWorld()->SpawnActor<AHMFirearmBase>(m_StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (m_CurrentWeapon)
		{
			m_CurrentWeapon->SetOwner(this);
			m_CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_WeaponAttachSocketName);
		}
	}
}

void AHMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMPlayerCharacter, m_CurrentWeapon);
	DOREPLIFETIME_CONDITION(AHMPlayerCharacter, m_bIsJumping, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHMPlayerCharacter, m_bIsSprinting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHMPlayerCharacter, m_bIsADS, COND_SkipOwner);
}

void AHMPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == EMovementMode::MOVE_Falling && GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		SetJumping(false);
	}
}

void AHMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_bIsSprinting && !IsSprinting())
	{
		SetSprinting(true);
	}

	m_FollowCamera->SetFieldOfView(FMath::FInterpTo(m_FollowCamera->FieldOfView, m_bIsADS ? m_ADSFOV : m_DefaultFOV, DeltaTime, 20.0f));
}

void AHMPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHMPlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHMPlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHMPlayerCharacter::StopCrouch);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AHMPlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AHMPlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AHMPlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AHMPlayerCharacter::StartReload);
	PlayerInputComponent->BindAction("ToggleFireMode", IE_Pressed, this, &AHMPlayerCharacter::ToggleFireMode);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHMPlayerCharacter::StartAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AHMPlayerCharacter::StopAttack);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AHMPlayerCharacter::StartADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &AHMPlayerCharacter::StopADS);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHMPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHMPlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHMPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHMPlayerCharacter::LookUpAtRate);
}

void AHMPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * m_BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHMPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * m_BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHMPlayerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHMPlayerCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AHMPlayerCharacter::StartCrouch()
{
	if (!bIsCrouched)
	{
		Crouch();
	}
}

void AHMPlayerCharacter::StopCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AHMPlayerCharacter::StartJump() { SetJumping(true); }

void AHMPlayerCharacter::SetJumping(bool bJumping)
{
	m_bIsJumping = bJumping;

	if (bJumping)
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}

		Jump();
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetJumping(bJumping);
	}
}

bool AHMPlayerCharacter::Server_SetJumping_Validate(bool bJumping) { return true; }
void AHMPlayerCharacter::Server_SetJumping_Implementation(bool bJumping) { SetJumping(bJumping); }

void AHMPlayerCharacter::StartSprint() { SetSprinting(true); }
void AHMPlayerCharacter::StopSprint() { SetSprinting(false); }

void AHMPlayerCharacter::SetSprinting(bool bSprint)
{
	m_bIsSprinting = bSprint;

	if (bIsCrouched)
	{
		UnCrouch();
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetSprinting(bSprint);
	}
}

bool AHMPlayerCharacter::Server_SetSprinting_Validate(bool bSprint) { return true; }
void AHMPlayerCharacter::Server_SetSprinting_Implementation(bool bSprint) { SetSprinting(bSprint); }


void AHMPlayerCharacter::StartADS() { SetADS(true); }
void AHMPlayerCharacter::StopADS() { SetADS(false); }

void AHMPlayerCharacter::SetADS(bool bADS)
{
	m_bIsADS = bADS;

	if (GetLocalRole() < ROLE_Authority)
	{
		Server_SetADS(bADS);
	}
}

bool AHMPlayerCharacter::Server_SetADS_Validate(bool bADS) { return true; }
void AHMPlayerCharacter::Server_SetADS_Implementation(bool bADS) { SetADS(bADS); }

FRotator AHMPlayerCharacter::GetAimOffsets() const
{
	return ActorToWorld().InverseTransformVectorNoScale(GetBaseAimRotation().Vector()).Rotation();
}

void AHMPlayerCharacter::StartAttack()
{
	if (m_CurrentWeapon)
	{
		m_CurrentWeapon->StartFire();
	}
}

void AHMPlayerCharacter::StopAttack()
{
	if (m_CurrentWeapon)
	{
		m_CurrentWeapon->StopFire();
	}
}

void AHMPlayerCharacter::StartReload()
{
	if (m_CurrentWeapon)
	{
		m_CurrentWeapon->StartReload();
	}
}

void AHMPlayerCharacter::ToggleFireMode()
{
	if (m_CurrentWeapon)
	{
		if (AHMFirearmBase* const Firearm = Cast<AHMFirearmBase>(m_CurrentWeapon))
		{
			if (Firearm->GetFirearmStats().AllowedFireModes.Num() > 1)
			{
				FFirearmStats FirearmStats = Firearm->GetFirearmStats();

				int32 index = FirearmStats.AllowedFireModes.Find(Firearm->GetFireMode());
				int32 idx = 0;

				if (FirearmStats.AllowedFireModes.IsValidIndex(index + 1))
				{
					idx = index + 1;
				}

				EFireMode NewMode = FirearmStats.AllowedFireModes[idx];

				Firearm->ToggleFireMode(NewMode);
			}
		}
	}
}

class AActor* AHMPlayerCharacter::GetActorInView()
{
	// If the controller for the character is null then return a nullptr instead of running the rest of the method
	if (GetController() == nullptr)
	{
		return nullptr;
	}

	// Get the players view point (what they're looking at)
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Calculate the end location for the line trace
	FVector EndLocation = CameraLocation + (CameraRotation.Vector() * m_MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceComplex = true;

	// Perform the line trace
	FHitResult HitRes;
	GetWorld()->LineTraceSingleByChannel(HitRes, CameraLocation, EndLocation, ECC_GameTraceChannel18, TraceParams);

#ifdef _DEBUG
	DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Yellow, false, 10.0f, 0, 1.0f);
#endif // _DEBUG

	return Cast<AActor>(HitRes.Actor);
}

void AHMPlayerCharacter::Interact()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Interact();
		return;
	}

	if (AActor* const actor = GetActorInView())
	{
		// If the actor that you're looking at implements the interface InteractInterface
		if (actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(actor, this);
		}
	}
}

bool AHMPlayerCharacter::Server_Interact_Validate() { return true; }
void AHMPlayerCharacter::Server_Interact_Implementation()
{
	Interact();
}

bool AHMPlayerCharacter::IsWeaponLocationAvailable(EWeaponAttachLocation Location)
{
	return nullptr == m_WeaponInventory.FindByPredicate([Location](AHMWeaponBase* Weapon)
	{
		return Weapon->GetAttachLocation() == Location;
	});
}

void AHMPlayerCharacter::AddWeapon(AHMWeaponBase* NewWeapon)
{
	if (IsWeaponLocationAvailable(NewWeapon->GetAttachLocation()))
	{
		NewWeapon->SetOwner(this);
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_WeaponAttachSocketName);

		m_CurrentWeapon = NewWeapon;
		// m_Inventory.Add(NewFirearm);
		PRINT("CALLED");
	}
}

