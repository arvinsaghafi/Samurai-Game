// katana.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Katana.generated.h"

UCLASS()
class SAMURAIGAME_API AKatana : public AActor
{
	GENERATED_BODY()
    
public: 
	// Sets default values for this actor's properties
	AKatana();

	void Attack();
	void EndAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public: 
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
					   bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* HitBox;

	UPROPERTY(EditAnywhere)
	UParticleSystem* SlashEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* SlashSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	float Damage = 25.0f;

	UPROPERTY()
	TArray<AActor*> HitActors;

	bool bIsAttacking;

	AController* GetOwnerController() const;
	void ResetHitActors();

	FTimerHandle AttackTimerHandle;
};