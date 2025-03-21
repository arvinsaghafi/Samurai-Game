// katana.cpp

#include "Katana.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

// Sets default values
AKatana::AKatana()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);

    // Create a hitbox for the katana
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    HitBox->SetupAttachment(Mesh);

    // Configure the hitbox
    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disabled by default, enabled during attacks
    HitBox->SetCollisionObjectType(ECC_WorldDynamic);
    HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Set the initial size of the hitbox
    HitBox->SetBoxExtent(FVector(5.0f, 30.0f, 2.0f)); // Adjust based on your katana model

    // Bind the overlap event
    HitBox->OnComponentBeginOverlap.AddDynamic(this, &AKatana::OnOverlapBegin);

    bIsAttacking = false;
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

void AKatana::Attack()
{
    // Enable the hitbox collision
    HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // Clear the hit actors array for a new attack
    ResetHitActors();

    // Set attacking state
    bIsAttacking = true;

    // Play slash effect and sound
    if (SlashEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(SlashEffect, Mesh, TEXT("SlashSocket"));
    }

    if (SlashSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SlashSound, GetActorLocation());
    }

    // Optional: You can add debug visualization for the hitbox
#if WITH_EDITOR
    if (GetWorld()->IsPlayInEditor())
    {
        DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(),
            HitBox->GetComponentQuat(), FColor::Red, false, 1.0f);
    }
#endif
}

void AKatana::EndAttack()
{
    // Disable the hitbox collision when attack ends
    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Reset attacking state
    bIsAttacking = false;
}

void AKatana::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // Don't hit self or owner
    if (OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    // Check if we already hit this actor during this attack (to prevent multiple hits)
    if (HitActors.Contains(OtherActor))
    {
        return;
    }

    // Add to the list of hit actors for this attack
    HitActors.Add(OtherActor);

    // Get the attacker (owner) name
    FString AttackerName = "Unknown";
    if (GetOwner())
    {
        AttackerName = GetOwner()->GetName();

        // Try to get a more friendly name if it's a player character
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter && OwnerCharacter->GetPlayerState())
        {
            AttackerName = OwnerCharacter->GetPlayerState()->GetPlayerName();
            if (AttackerName.IsEmpty())
            {
                AttackerName = GetOwner()->GetName();
            }
        }
    }

    // Determine if the hit actor is a player character
    bool bIsPlayerCharacter = false;
    FString TargetName = OtherActor->GetName();

    ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
    if (HitCharacter)
    {
        bIsPlayerCharacter = true;
        // Try to get the player name if it has a player state
        if (HitCharacter->GetPlayerState())
        {
            FString PlayerName = HitCharacter->GetPlayerState()->GetPlayerName();
            if (!PlayerName.IsEmpty())
            {
                TargetName = PlayerName;
            }
        }

        // Log a special message for player hits
        UE_LOG(LogTemp, Warning, TEXT("*** PLAYER HIT: %s hit player %s with katana for %.1f damage! ***"),
            *AttackerName, *TargetName, Damage);
    }
    else
    {
        // Regular hit log
        UE_LOG(LogTemp, Warning, TEXT("Katana hit: %s"), *OtherActor->GetName());
    }

    // Play impact effect and sound
    if (ImpactEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, SweepResult.Location);
    }

    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, SweepResult.Location);
    }

    // Apply damage to the hit actor
    AController* OwnerController = GetOwnerController();
    if (OwnerController)
    {
        FPointDamageEvent DamageEvent(Damage, SweepResult, -SweepResult.Normal, nullptr);
        OtherActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
    }
}

AController* AKatana::GetOwnerController() const
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr) return nullptr;
    return OwnerPawn->GetController();
}

void AKatana::ResetHitActors()
{
    HitActors.Empty();
}