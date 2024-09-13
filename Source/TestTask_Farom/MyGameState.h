#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

UCLASS()
class TESTTASK_FAROM_API AMyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMyGameState();

    UPROPERTY(ReplicatedUsing = OnRep_Player1Score, BlueprintReadOnly, Category = "Score")
    int32 Player1Score;

    UPROPERTY(ReplicatedUsing = OnRep_Player2Score, BlueprintReadOnly, Category = "Score")
    int32 Player2Score;

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScoreToPlayer1();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScoreToPlayer2();

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetPlayer1Score() const;

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetPlayer2Score() const;

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
    UFUNCTION()
    void OnRep_Player1Score();

    UFUNCTION()
    void OnRep_Player2Score();
};