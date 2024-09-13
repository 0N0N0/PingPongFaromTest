// Fill out your copyright notice in the Description page of Project Settings.


#include "Racquet.h"
#include "Components/InputComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"

ARacquet::ARacquet()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    LastMoveRightValue = 0.0f;

    MinY = 13194.f;
    MaxY = 13646.f;

    DynamicMaterial = nullptr;
    MaterialColor = FLinearColor::Red;

    LastColorChangeTime = -ColorChangeCooldown;
}

void ARacquet::BeginPlay()
{
	Super::BeginPlay();
	
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        PlayerController->bAutoManageActiveCameraTarget = false;
        PlayerController->SetViewTarget(this);
    }

    UPrimitiveComponent* CollisionComponent = FindComponentByClass<UPrimitiveComponent>();

    if (CollisionComponent)
    {
        CollisionComponent->OnComponentHit.AddDynamic(this, &ARacquet::OnRacquetHit);

        UStaticMeshComponent* StaticMeshComponent = nullptr;

        for (int32 i = 0; i < CollisionComponent->GetNumChildrenComponents(); ++i)
        {
            UActorComponent* ChildComponent = CollisionComponent->GetChildComponent(i);
            StaticMeshComponent = Cast<UStaticMeshComponent>(ChildComponent);

            if (StaticMeshComponent)
            {
                break;
            }
        }

        if (StaticMeshComponent)
        {
            StaticMeshComponent->OnComponentHit.AddDynamic(this, &ARacquet::OnRacquetHit);

            UMaterialInterface* Material = StaticMeshComponent->GetMaterial(0);
            if (Material)
            {
                DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
                StaticMeshComponent->SetMaterial(0, DynamicMaterial);
            }
        }
    }

    if (HasAuthority())
    {
        SetMaterialColor();
    }
    else
    {
        OnRep_MaterialColor();
    }
}

void ARacquet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARacquet::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveRight", this, &ARacquet::MoveRight);
}

void ARacquet::MoveRight(float Value)
{
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        FVector RightDirection = GetActorRightVector();
        FVector Movement = RightDirection * Value * MovementSpeed * GetWorld()->DeltaTimeSeconds;
        FVector NewLocation = GetActorLocation() + Movement;

        float ClampedY = FMath::Clamp(NewLocation.Y, MinY, MaxY);
        NewLocation.Y = ClampedY;

        if (IsLocallyControlled())
        {
            SetActorLocation(NewLocation);
        }

        if (HasAuthority())
        {
            SetActorLocation(NewLocation); 
            LastMoveRightValue = Value;
        }
        else
        {
            ServerMoveRight(Value);
        }
    }
}

void ARacquet::ServerMoveRight_Implementation(float Value)
{
	MoveRight(Value);
}

void ARacquet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARacquet, LastMoveRightValue);
    DOREPLIFETIME(ARacquet, MaterialColor);
}

void ARacquet::OnRacquetHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (HasAuthority())
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - LastColorChangeTime >= ColorChangeCooldown)
        {
            SetMaterialColor();
            LastColorChangeTime = CurrentTime;
        }
    }
}

void ARacquet::SetMaterialColor()
{
    if (HasAuthority())
    {
        float RandomR = FMath::RandRange(0.0f, 1.0f);
        float RandomG = FMath::RandRange(0.0f, 1.0f);
        float RandomB = FMath::RandRange(0.0f, 1.0f);

        MaterialColor = FLinearColor(RandomR, RandomG, RandomB);
    }

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), MaterialColor);
    }
}

void ARacquet::OnRep_MaterialColor()
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), MaterialColor);
    }
}