#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AGun;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Shoot();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	void NextWeapon();
	void PreviousWeapon();

	void SwitchToWeapon(int32 NewIndex);

	UPROPERTY(EditAnywhere)
	float RotationRate = 70.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AGun>> GunClasses;

	UPROPERTY()
	TArray<AGun*> Guns;

	UPROPERTY()
	int32 ActiveIndex;

	UPROPERTY()
	AGun* Gun;
};
