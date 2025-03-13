// Fill out your copyright notice in the Description page of Project Settings.

#include "Katana.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"

// Sets default values
AKatana::AKatana()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//bReplicates(true);
	//SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	
	//Mesh->SetIsReplicated(true);
	//Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void AKatana::PullTrigger()
{
	//UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	//UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));
	
	//bool bSuccess = GunTrace(Hit, ShotDirection);

	TArray<AActor*> OverlappingActors;

	Mesh->GetOverlappingActors(OverlappingActors);
	
	bool bSuccess = IsColliding(OverlappingActors);

	if (bSuccess)
	{
		{
				//FPointDamageEvent DamageEvent(Damage, FHitResult(), FVector::ZeroVector, nullptr);
				//AController* OwnerController = GetOwnerController();
				//HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);//
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Something is wrong."));
	}
	
	/*if (bSuccess) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);

		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr) {

			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			AController *OwnerController = GetOwnerController();
			HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
		}
	}*/
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKatana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*bool AKatana::GunTrace(FHitResult &Hit, FVector& ShotDirection)
{
	AController *OwnerController = GetOwnerController();
	if (OwnerController == nullptr) return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector End = Location + Rotation.Vector() * MaxRange;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel1, Params);
}*/

AController* AKatana::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return nullptr;
	return OwnerPawn->GetController();
}

bool AKatana::IsColliding(TArray<AActor*> OverlappingActors)
{
	for (AActor* Actor : OverlappingActors) 
	{
		if (Actor != this && Actor != GetOwner()) // Ignore self and owner
		{
			//UE_LOG(LogTemp, Warning, TEXT("Katana hit: %s"), *HitActor->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Something was hit"));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Katana did not hit anything."));
		}
	}
	return false;
}