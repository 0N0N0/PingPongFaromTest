#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class TESTTASK_FAROM_API ABall : public AActor
{
    GENERATED_BODY()

public:
    ABall();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball", ReplicatedUsing = OnRep_BallVelocity)
    FVector BallVelocity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
    float BounceScale;

protected:

    UFUNCTION()
    void OnBallOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void StartMovement();

    void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

    void ResetBall();

private:
    FTimerHandle StartMovementTimerHandle;

    UFUNCTION()
    void OnRep_BallVelocity();

    FVector SpawnLocation;
};
