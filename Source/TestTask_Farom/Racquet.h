// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Racquet.generated.h"


UCLASS()
class TESTTASK_FAROM_API ARacquet : public APawn
{
	GENERATED_BODY()

public:
	ARacquet();

	virtual void Tick(float DeltaTime) override;

	void MoveRight(float Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed = 500.0f;

	UFUNCTION()
	void OnRep_MaterialColor();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRacquetHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UFUNCTION(Server, Reliable)
	void ServerMoveRight(float Value);

	void ServerMoveRight_Implementation(float Value);
	bool ServerMoveRight_Validate(float Value) const;

	UPROPERTY(Replicated)
	float LastMoveRightValue;

	float MinY;
	float MaxY;

	void SetMaterialColor();

	UPROPERTY(ReplicatedUsing = OnRep_MaterialColor)
	FLinearColor MaterialColor;

	UMaterialInstanceDynamic* DynamicMaterial;

	float LastColorChangeTime;
	const float ColorChangeCooldown = 0.5f;
};
