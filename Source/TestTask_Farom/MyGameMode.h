// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

UCLASS()
class TESTTASK_FAROM_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void AddScoreToPlayer1();
	void AddScoreToPlayer2();

	void EndGame(int WinningPlayer);

	void PlayerPressedPlay(APlayerController* PlayerController);
	void CheckPlayersReady();


private:
	void StartGame();

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ARacquet> RacquetBlueprint;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ABall> BallBlueprint;

	int32 PlayerCount;

	UPROPERTY()
	APlayerController* WaitingPlayer;

	int32 PlayerReadyCount = 0;

	void UpdateScoreWidgets();
};
