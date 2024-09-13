// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Racquet.h"

#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "MyGameState.h"


void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAxis("MoveRight", this, &AMyPlayerController::MoveRight);
}

void AMyPlayerController::MoveRight(float Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ARacquet* Racquet = Cast<ARacquet>(ControlledPawn))
        {
            Racquet->MoveRight(Value);
        }
    }
}

void AMyPlayerController::ShowWaitingForMatchWidget()
{
	if (!WaitingForMatchWidgetClass)
	{
		WaitingForMatchWidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("/Game/Widgets/WBP_WaitingForMatch.WBP_WaitingForMatch_C")));
	}

	if (WaitingForMatchWidgetClass)
	{
		if (!WaitingForMatchWidget)
		{
			WaitingForMatchWidget = CreateWidget<UUserWidget>(this, WaitingForMatchWidgetClass);
		}
		
		if (WaitingForMatchWidget)
		{
			WaitingForMatchWidget->AddToViewport();
		}
	}
}

void AMyPlayerController::ClientShowWaitingForMatch_Implementation()
{
	ShowWaitingForMatchWidget();
}


void AMyPlayerController::HideWaitingForMatchWidget()
{
	if (IsValid(WaitingForMatchWidget))
	{
		WaitingForMatchWidget->RemoveFromViewport();
		WaitingForMatchWidget->Destruct();
		WaitingForMatchWidget = nullptr;
	}
}

void AMyPlayerController::ClientHideWaitingForMatch_Implementation()
{
	HideWaitingForMatchWidget();
}


void AMyPlayerController::ShowScoreWidget()
{
	if (!ScoreWidgetClass)
	{
		ScoreWidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("/Game/Widgets/WBP_Score.WBP_Score_C")));
	}

	if (ScoreWidgetClass)
	{
		if (!ScoreWidget)
		{
			ScoreWidget = CreateWidget<UUserWidget>(this, ScoreWidgetClass);
		}

		if (ScoreWidget)
		{
            ScoreWidget->SetOwningPlayer(this);
			ScoreWidget->AddToViewport();
		}
	}
}

void AMyPlayerController::ClientShowScore_Implementation()
{
	ShowScoreWidget();
}


void AMyPlayerController::ShowMainMenu()
{
    if (!MainMenuWidgetClass)
    {
        MainMenuWidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("/Game/Widgets/WBP_Menu.WBP_Menu_C")));
    }

    if (MainMenuWidgetClass)
    {
        if (!MainMenuWidget)
        {
            MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        }

        if (MainMenuWidget)
        {
            MainMenuWidget->SetOwningPlayer(this);
            MainMenuWidget->AddToViewport();

            FInputModeUIOnly InputMode;
            SetInputMode(InputMode);
            bShowMouseCursor = true;
        }
    }
}

void AMyPlayerController::ClientShowMainMenu_Implementation()
{
    ShowMainMenu();
}


void AMyPlayerController::HideMainMenu()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->RemoveFromViewport();
        MainMenuWidget = nullptr;

        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = false;
    }
}

void AMyPlayerController::ClientHideMainMenu_Implementation()
{
    HideMainMenu();
}

void AMyPlayerController::ServerPlayerPressedPlay_Implementation()
{
    AMyGameMode* GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->PlayerPressedPlay(this);
    }
}

void AMyPlayerController::UpdateScoreWidget()
{
    if (!ScoreWidget)
    {
        return;
    }

    AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>();
    if (MyGameState)
    {
        UFunction* UpdateScoreFunc = ScoreWidget->FindFunction(FName("UpdateScoreText"));
        if (UpdateScoreFunc)
        {
            struct FScoreParams
            {
                int32 Score1;
                int32 Score2;
            };

            FScoreParams Params;
            if (PlayerIndex == 1)
            {
                Params.Score1 = MyGameState->GetPlayer1Score();
                Params.Score2 = MyGameState->GetPlayer2Score();
            }
            else
            {
                Params.Score1 = MyGameState->GetPlayer2Score();
                Params.Score2 = MyGameState->GetPlayer1Score();
            }

            ScoreWidget->ProcessEvent(UpdateScoreFunc, &Params);
        }
    }
}

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyPlayerController, PlayerIndex);
}