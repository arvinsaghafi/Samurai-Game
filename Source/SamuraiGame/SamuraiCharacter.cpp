// Fill out your copyright notice in the Description page of Project Settings.

#include "SamuraiCharacter.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Katana.h"
#include "Components/CapsuleComponent.h"
#include "SamuraiGameGameMode.h"

// Sets default values
ASamuraiCharacter::ASamuraiCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASamuraiCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Make sure KatanaClass is set in the Blueprint
	if (KatanaClass)
	{
		// Spawn the katana
		Katana = GetWorld()->SpawnActor<AKatana>(KatanaClass);

		// Check if spawn was successful
		if (Katana)
		{
			// Attach katana to the character's hand socket
			Katana->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
			Katana->SetOwner(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Katana!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaClass is not set in Blueprint!"));
	}
}

bool ASamuraiCharacter::IsDead() const
{
	return Health <= 0;
}

float ASamuraiCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void ASamuraiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASamuraiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Call the parent version
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player controller is invalid!"));
		return;
	}

	// Get the local player enhanced input subsystem
	auto EISubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	if (!EISubSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Enhanced Input Subsystem is invalid!"));
		return;
	}

	if (!InputMapping)
	{
		UE_LOG(LogTemp, Error, TEXT("InputMapping is not set in Blueprint!"));
		return;
	}

	// Clear out existing mapping, add our mapping
	EISubSystem->ClearAllMappings();
	// Add the input mapping context
	EISubSystem->AddMappingContext(InputMapping, 0);

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PlayerEIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerEIComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Component initialized."));

		// Check if input actions are set in Blueprint
		if (!InputMove || !InputLook || !InputJump || !InputShoot)
		{
			UE_LOG(LogTemp, Error, TEXT("One or more Input Actions are not set in Blueprint!"));
			return;
		}

		// Bind the actions
		PlayerEIComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &ASamuraiCharacter::Move);
		PlayerEIComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &ASamuraiCharacter::Look);
		PlayerEIComponent->BindAction(InputJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		PlayerEIComponent->BindAction(InputShoot, ETriggerEvent::Started, this, &ASamuraiCharacter::Attack);

		if (InputTargetLock)
		{
			PlayerEIComponent->BindAction(InputTargetLock, ETriggerEvent::Started, this, &ASamuraiCharacter::TargetLock);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enhanced Input Component initialization failed."));
	}
}

float ASamuraiCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	if (IsDead()) {

		ASamuraiGameGameMode* GameMode = GetWorld()->GetAuthGameMode<ASamuraiGameGameMode>();
		if (GameMode != nullptr) {

			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

void ASamuraiCharacter::Move(const FInputActionValue& Value) {
	// Directional movement
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASamuraiCharacter::Look(const FInputActionValue& Value) {
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASamuraiCharacter::Attack()
{
	// Check if Katana exists before using it
	if (Katana)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attacking with katana"));
		Katana->Attack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Attack attempted but Katana is null!"));
	}
}

void ASamuraiCharacter::TargetLock()
{
	// Implement target locking functionality
	UE_LOG(LogTemp, Warning, TEXT("Target lock not implemented yet"));
}