// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class TESTTASK_FAROM_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientShowWaitingForMatch();

	UFUNCTION(Client, Reliable)
	void ClientHideWaitingForMatch();

	UFUNCTION(Client, Reliable)
	void ClientShowScore();

	UFUNCTION(Client, Reliable)
	void ClientShowMainMenu();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerPlayerPressedPlay();

	UFUNCTION(Client, Reliable)
	void ClientHideMainMenu();

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 PlayerIndex;

	void UpdateScoreWidget();

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WaitingForMatchWidgetClass;

	UPROPERTY()
	UUserWidget* WaitingForMatchWidget;

	void ShowWaitingForMatchWidget();

	void HideWaitingForMatchWidget();

	void ShowScoreWidget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ScoreWidgetClass;

	UPROPERTY()
	UUserWidget* ScoreWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UUserWidget* MainMenuWidget;

	void ShowMainMenu();
	void HideMainMenu();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void MoveRight(float Value);
};
