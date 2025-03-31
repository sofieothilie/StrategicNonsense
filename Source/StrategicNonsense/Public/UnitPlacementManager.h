#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnitPlacementManager.generated.h"

/**
 * @class AUnitPlacementManager
 * @brief Manages the unit placement phase at the start of the game.
 *
 * Alternates between player and AI placement turns.
 * Handles unit instantiation, placement validation, and widget triggering.
 */


class UTeam;
class AGridManager;
class ABattleGameMode;
class AUnitActor;
class UStartMessageWidget;
class UUnitSelectionWidget;

UCLASS()
class STRATEGICNONSENSE_API UUnitPlacementManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialise(ABattleGameMode* InGameMode, AGridManager* InGridManager, const TArray<UTeam*>& Teams, bool bPlayerStarts);

    void HandlePlayerClickedGrid(const FVector& ClickLocation);

    UFUNCTION()
    void HandleUnitChosen(TSubclassOf<AUnitActor> ChosenUnit);
    void StartNextPlacementStep();

private:
    void PlaceAIUnit();
    void RemoveUnitFromQueue(TSubclassOf<AUnitActor> UnitClass, UTeam* OwningTeam);
    void ShowStartMessage();


    UPROPERTY()
    ABattleGameMode* GameMode;

    UPROPERTY()
    AGridManager* GridManager;

    UPROPERTY()
    TArray<UTeam*> AllTeams;

    UPROPERTY()
    UTeam* TeamPlacingNext;

    UPROPERTY()
    TSubclassOf<AUnitActor> UnitToPlaceNext;

    UPROPERTY()
    UClass* UnitSelectionWidgetClass = nullptr;

    int32 CurrentPlacementTeamIndex = 0;
    bool bUnitSelectionWidgetShown = false;
    FText StartText;

};
