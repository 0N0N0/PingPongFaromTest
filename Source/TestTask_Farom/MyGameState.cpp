#include "MyGameState.h"
#include "Net/UnrealNetwork.h"
#include "MyPlayerController.h"

AMyGameState::AMyGameState()
{
    Player1Score = 0;
    Player2Score = 0;
}

void AMyGameState::AddScoreToPlayer1()
{
    Player1Score++;
}

void AMyGameState::AddScoreToPlayer2()
{
    Player2Score++;
}

int32 AMyGameState::GetPlayer1Score() const
{
    return Player1Score;
}

int32 AMyGameState::GetPlayer2Score() const
{
    return Player2Score;
}

void AMyGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AMyGameState, Player1Score);
    DOREPLIFETIME(AMyGameState, Player2Score);
}

void AMyGameState::OnRep_Player1Score()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
        {
            MyPlayerController->UpdateScoreWidget();
        }
    }
}

void AMyGameState::OnRep_Player2Score()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
        {
            MyPlayerController->UpdateScoreWidget();
        }
    }
}