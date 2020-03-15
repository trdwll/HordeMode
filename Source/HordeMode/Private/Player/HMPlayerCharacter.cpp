// Copyright (c) 2020 Russ 'trdwll' Treadwell

#include "Player/HMPlayerCharacter.h"
#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Interfaces/Interactable.h"
#include "HordeMode.h"

#include "Net/UnrealNetwork.h"

#ifdef _DEBUG
#include "Engine.h"
#endif // _DEBUG

AHMPlayerCharacter::AHMPlayerCharacter()
	: m_BaseTurnRate(45.0f), m_BaseLookUpRate(45.0f), m_MaxUseDistance(380.0f), m_Currency(1000)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

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

void AHMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHMPlayerCharacter, m_Currency);
}

void AHMPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHMPlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHMPlayerCharacter::StopCrouch);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AHMPlayerCharacter::Interact);

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

class AActor* AHMPlayerCharacter::GetActorInView()
{
	ACharacter* const Character = Cast<ACharacter>(this);

	// If the character or the controller for the character are null then return a nullptr instead of running the rest of the method
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return nullptr;
	}

	// Get the players view point (what they're looking at)
	FVector CameraLocation;
	FRotator CameraRotation;
	Character->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

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
	if (AActor* const actor = GetActorInView())
	{
		// If the actor that you're looking at implements the interface InteractInterface
		if (actor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			IInteractable::Execute_Interact(actor);
		}
	}
}

void AHMPlayerCharacter::AddCurrency(int32 CurrencyToAdd)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_AddCurrency(CurrencyToAdd);
	}

	m_Currency += CurrencyToAdd;

	OnCharacterCurrencyChange.Broadcast(m_Currency);
}

bool AHMPlayerCharacter::Server_AddCurrency_Validate(int32 CurrencyToAdd) { return true; }
void AHMPlayerCharacter::Server_AddCurrency_Implementation(int32 CurrencyToAdd) { AddCurrency(CurrencyToAdd); }
