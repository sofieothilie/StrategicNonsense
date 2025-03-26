#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class AGridManager;
class AUnitActor;
class UTeam;
class UUnitSelectionWidget;
class UStartMessageWidget;

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

private:
    void SpawnTopDownCamera();
    void SpawnGridAndSetup();
    void SetupTeams();
    void SpawnTeamUnits();
    void DecideStartingPlayer();
    void ShowUnitSelection(UTeam* CurrentTeam);
    void PromptNextUnitSelection();
    void PlaceAIUnit();

    UFUNCTION()
    void HandleUnitChosen(TSubclassOf<AUnitActor> ChosenUnit);

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

    UPROPERTY()
    TArray<FUnitPlacementEntry> UnitsToPlace;

    UPROPERTY()
    TSubclassOf<AUnitActor> UnitToPlaceNext;

    UPROPERTY()
    UTeam* TeamPlacingNext;

    int32 CurrentPlacementTeamIndex = 0;

    bool bPlayerStarts = false;
    bool bUnitSelectionWidgetShown = false;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UStartMessageWidget> StartMessageWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUnitSelectionWidget> UnitSelectionWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AGridManager> GridManagerClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> CellBlueprint;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> BP_Tree1;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> BP_Tree2;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> BP_Mountain;
};