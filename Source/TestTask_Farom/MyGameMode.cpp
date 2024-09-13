// Fill out your copyright notice in the Description page of Project Settings.
#include "MyGameMode.h"
#include "MyGameState.h"
#include "MyPlayerController.h"
#include "Racquet.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Ball.h"

AMyGameMode::AMyGameMode()
{
    PlayerControllerClass = AMyPlayerController::StaticClass();
    DefaultPawnClass = ARacquet::StaticClass();
    GameStateClass = AMyGameState::StaticClass();

    PlayerCount = 0;

    if (!RacquetBlueprint)
    {
        static ConstructorHelpers::FClassFinder<ARacquet> RacquetBPClass(TEXT("/Game/Blueprints/BP_Racquet"));
        if (RacquetBPClass.Class != nullptr)
        {
            RacquetBlueprint = RacquetBPClass.Class;
        }
    }

    if (!BallBlueprint)
    {
        static ConstructorHelpers::FClassFinder<ABall> BallBPClass(TEXT("/Game/Blueprints/BP_Ball"));
        if (BallBPClass.Class != nullptr)
        {
            BallBlueprint = BallBPClass.Class;
        }
    }
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    AMyPlayerController* PlayerController1 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    AMyPlayerController* PlayerController2 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 1));


    if (PlayerController1)
    {
        PlayerController1->PlayerIndex = 0;
        PlayerController1->ClientShowMainMenu();
    }

    if (PlayerController2)
    {
        PlayerController2->PlayerIndex = 1;
        PlayerController2->ClientShowMainMenu();
    }
}

void AMyGameMode::PlayerPressedPlay(APlayerController* PlayerController)
{
    PlayerReadyCount++;

    AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController);
    if (MyPlayerController)
    {
        if (PlayerReadyCount == 1)
        {
            MyPlayerController->ClientShowWaitingForMatch();
        }
        else if (PlayerReadyCount == 2)
        {
            CheckPlayersReady();
        }
    }
}

void AMyGameMode::CheckPlayersReady()
{
    if (PlayerReadyCount == 2)
    {
        AMyPlayerController* PlayerController1 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        AMyPlayerController* PlayerController2 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 1));

        if (PlayerController1 && PlayerController2)
        {
            PlayerController1->ClientHideWaitingForMatch();
            PlayerController2->ClientHideWaitingForMatch();

            StartGame();
        }
    }
}

void AMyGameMode::StartGame()
{
    if (RacquetBlueprint == nullptr || BallBlueprint == nullptr)
    {
        return;
    }

    AMyPlayerController* PlayerController1 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    AMyPlayerController* PlayerController2 = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 1));

    if (PlayerController1 && PlayerController2)
    {
        PlayerController1->ClientHideMainMenu();
        PlayerController2->ClientHideMainMenu();

        FVector Player1SpawnLocation(125520.f, 13390.f, 99590.f);
        FVector Player2SpawnLocation(126080.f, 13390.f, 99590.f);

        FActorSpawnParameters SpawnParams;
        
        ARacquet* Racquet1 = GetWorld()->SpawnActor<ARacquet>(RacquetBlueprint, Player1SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        
        FRotator Player2Rotation(0.0f, 180.0f, 0.0f);
        ARacquet* Racquet2 = GetWorld()->SpawnActor<ARacquet>(RacquetBlueprint, Player2SpawnLocation, Player2Rotation, SpawnParams);

        if (Racquet1 && Racquet2)
        {
            PlayerController1->Possess(Racquet1);
            PlayerController2->Possess(Racquet2);
        }


        FVector BallSpawnLocation(125770.f, 13420.f, 99570.f);
        ABall* Ball = GetWorld()->SpawnActor<ABall>(BallBlueprint, BallSpawnLocation, FRotator::ZeroRotator, SpawnParams);


        PlayerController1->ClientShowScore();
        PlayerController2->ClientShowScore();
    }
}

void AMyGameMode::AddScoreToPlayer1()
{
    AMyGameState* MyGameState = GetGameState<AMyGameState>();
    if (MyGameState)
    {
        MyGameState->AddScoreToPlayer1();
    }

    if (MyGameState->GetPlayer1Score() >= 10)
    {
        EndGame(1);
    }
}

void AMyGameMode::AddScoreToPlayer2()
{
    AMyGameState* MyGameState = GetGameState<AMyGameState>();
    if (MyGameState)
    {
        MyGameState->AddScoreToPlayer2();
    }

    if (MyGameState->GetPlayer2Score() >= 10)
    {
        EndGame(2);
    }
}

void AMyGameMode::EndGame(int WinningPlayer)
{
    if (WinningPlayer == 1)
    {
        UE_LOG(LogTemp, Log, TEXT("Player 1 Wins!"));
    }
    else if (WinningPlayer == 2)
    {
        UE_LOG(LogTemp, Log, TEXT("Player 2 Wins!"));
    }
}