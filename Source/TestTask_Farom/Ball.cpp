#include "Ball.h"
#include "Racquet.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "MyGameMode.h"

ABall::ABall()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    if (HasAuthority())
    {
        BallVelocity = FVector(0.0f, 0.0f, 0.0f);
        BounceScale = 1.0f;
    }

    SpawnLocation = FVector(125770.f, 13420.f, 99570.f);
}

void ABall::BeginPlay()
{
    Super::BeginPlay();

    UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();

    if (PrimitiveComponent)
    {
        PrimitiveComponent->SetSimulatePhysics(true);

        PrimitiveComponent->SetLinearDamping(0.0f);
        PrimitiveComponent->SetAngularDamping(0.0f);

        UPhysicalMaterial* PhysMaterial = PrimitiveComponent->GetBodyInstance()->GetSimplePhysicalMaterial();
        if (PhysMaterial)
        {
            PhysMaterial->Friction = 0.0f;
            PhysMaterial->Restitution = 1.0f;
        }

        PrimitiveComponent->OnComponentHit.AddDynamic(this, &ABall::OnBallHit);
        PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ABall::OnBallOverlap);
    }

    GetWorld()->GetTimerManager().SetTimer(StartMovementTimerHandle, this, &ABall::StartMovement, 3.0f, false);
}

void ABall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();

    if (PrimitiveComponent && BallVelocity.SizeSquared() > KINDA_SMALL_NUMBER)
    {
        const float DesiredSpeed = 500.f;

        if (BallVelocity.Size() > DesiredSpeed)
        {
            BallVelocity = BallVelocity.GetSafeNormal() * DesiredSpeed;
        }

        FVector CurrentVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
        if (FMath::Abs(CurrentVelocity.Size() - BallVelocity.Size()) > 100.f)
        {
            BallVelocity = CurrentVelocity.GetSafeNormal() * DesiredSpeed;
        }

        PrimitiveComponent->SetPhysicsLinearVelocity(BallVelocity);
    }
}

void ABall::StartMovement()
{
    if (HasAuthority())
    {
        UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();
        if (PrimitiveComponent)
        {
            BallVelocity = FVector(500.f, 0.f, 0.0f);
            PrimitiveComponent->SetPhysicsLinearVelocity(BallVelocity);
        }
    }
}

void ABall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABall, BallVelocity);
}

void ABall::ResetBall()
{
    UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();
    if (PrimitiveComponent)
    {
        PrimitiveComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
        PrimitiveComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    }

    SetActorLocation(SpawnLocation);

    GetWorld()->GetTimerManager().SetTimer(StartMovementTimerHandle, this, &ABall::StartMovement, 3.0f, false);
}

void ABall::OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority())
    {
        UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();

        if (PrimitiveComponent)
        {
            FVector ReflectedVelocity = BallVelocity.MirrorByVector(Hit.Normal);

            const float DesiredSpeed = 500.f;
            ReflectedVelocity = ReflectedVelocity.GetSafeNormal() * DesiredSpeed;

            FVector OtherVelocity = FVector::ZeroVector;
            if (OtherComponent)
            {
                OtherVelocity = OtherComponent->GetPhysicsLinearVelocity();
            }

            BallVelocity = ReflectedVelocity + OtherVelocity * 0.8;

            PrimitiveComponent->SetPhysicsLinearVelocity(BallVelocity);
        }
    }
}

void ABall::OnBallOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->ActorHasTag(FName("Player1Goal")))
    {
        AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
        if (GameMode)
        {
            GameMode->AddScoreToPlayer2();
        }
    }
    else if (OtherActor->ActorHasTag(FName("Player2Goal")))
    {
        AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
        if (GameMode)
        {
            GameMode->AddScoreToPlayer1();
        }
    }

    ResetBall();
}

UFUNCTION()
void ABall::OnRep_BallVelocity()
{
    UPrimitiveComponent* PrimitiveComponent = FindComponentByClass<UPrimitiveComponent>();
    if (PrimitiveComponent)
    {
        PrimitiveComponent->SetPhysicsLinearVelocity(BallVelocity);
    }
}