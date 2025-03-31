#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CombatManager.h"
#include "BattleGameMode.generated.h"


class AGridManager;
class AUnitActor;
class UTeam;
class UUnitSelectionWidget;
class UStartMessageWidget;
class UUnitPlacementManager;
class UCombatManager;
class GameOverWidget;
class UGameStatusWidget;
class UEndTurnWidget;

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    Placement,
    PlayerTurn,
    AITurn,
    GameOver
};

USTRUCT()
struct FUnitPlacementEntry
{
    GENERATED_BODY()

    UPROPERTY()
    TSubclassOf<AUnitActor> UnitClass;

    UPROPERTY()
    UTeam* OwningTeam;
};

UCLASS()
class STRATEGICNONSENSE_API ABattleGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABattleGameMode();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    bool DoesPlayerStart() const { return bPlayerStarts; }

    void OnPlayerClickedGrid(const FVector& ClickLocation);
    void SetGamePhase(EGamePhase NewPhase);
    EGamePhase GetCurrentPhase() const { return CurrentPhase; }
    UTeam* GetPlayerTeam() const;
    UTeam* GetAITeam() const;

    void HandleAITurn();


    UPROPERTY()
    UCombatManager* CombatManager;

    void CheckGameEnd();

    UFUNCTION()
    void SpawnGameStatusWidget();

    UFUNCTION()
    void UpdateGameStatusWidget();


    UPROPERTY()
    TSubclassOf<UEndTurnWidget> EndTurnWidgetClass;


private:
    void SpawnTopDownCamera();
    void SpawnGridAndSetup();
    void SetupTeams();
    void DecideStartingPlayer();

private:
    EGamePhase CurrentPhase = EGamePhase::Placement;

    UPROPERTY()
    TArray<UTeam*> AllTeams;

    UPROPERTY()
    UTeam* Team1;

    UPROPERTY()
    UTeam* Team2;

    UPROPERTY()
    AGridManager* SpawnedGridManager;

    bool bPlayerStarts = false;

    UPROPERTY()
    UUnitPlacementManager* UnitPlacementManager;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY()
    UUserWidget* GameOverWidget;

    void ShowGameOverWidget(const FString& ResultText);

    UPROPERTY()
    UGameStatusWidget* GameStatusWidget;


};