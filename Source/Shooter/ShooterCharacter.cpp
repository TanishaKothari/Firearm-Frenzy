#include "ShooterCharacter.h"
#include "Gun.h"
#include "ShooterGameModeBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	for (TSubclassOf<AGun> GunClass : GunClasses)
	{
		AGun* Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		if (Gun)
		{
			Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
			Gun->SetOwner(this);
			Guns.Add(Gun);
			Gun->SetActorHiddenInGame(true);
		}
	}

	if (Guns.Num() > 0)
	{
		ActiveIndex = 0;
		Guns[ActiveIndex]->SetActorHiddenInGame(false);
	}
}

bool AShooterCharacter::IsDead() const
{
	// Check if player is dead or alive
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("NextWeapon"), IE_Pressed, this, &AShooterCharacter::NextWeapon);
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), IE_Pressed, this, &AShooterCharacter::PreviousWeapon);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;

	if (IsDead())
	{
		AShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AShooterGameModeBase>();
		if (GameMode)
		{
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	return DamageToApply;
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::NextWeapon()
{
	int32 NewIndex = (ActiveIndex + 1) % Guns.Num();
	SwitchToWeapon(NewIndex);
}

void AShooterCharacter::PreviousWeapon()
{
	int32 NewIndex = (ActiveIndex - 1 + Guns.Num()) % Guns.Num();
	SwitchToWeapon(NewIndex);
}

void AShooterCharacter::SwitchToWeapon(int32 NewIndex)
{
	if (NewIndex != ActiveIndex && Guns.IsValidIndex(NewIndex))
	{
		Guns[ActiveIndex]->SetActorHiddenInGame(true);
		ActiveIndex = NewIndex;
		Guns[ActiveIndex]->SetActorHiddenInGame(false);
	}
}

void AShooterCharacter::Shoot()
{
	if (Guns.IsValidIndex(ActiveIndex) && Guns[ActiveIndex])
	{
		Guns[ActiveIndex]->PullTrigger();
	}
}
