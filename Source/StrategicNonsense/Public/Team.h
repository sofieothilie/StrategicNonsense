#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Team.generated.h"

/**
 * @class ATeam
 * @brief Represents a team containing one Sniper and one Brawler.
 *
 * Each team has a unique colour and contains exactly two unit references.
 * Used for placement, gameplay logic, and UI updates.
 */


class AUnitActor;

UCLASS(Blueprintable)
class STRATEGICNONSENSE_API UTeam : public UObject
{
    GENERATED_BODY()

public:
    void Initialise(FName Colour, bool bIsPlayer);

    TSubclassOf<AUnitActor> GetSniperBlueprint() const;
    TSubclassOf<AUnitActor> GetBrawlerBlueprint() const;

    TArray<TSubclassOf<AUnitActor>>& GetUnplacedUnits();


    bool IsPlayerControlled() const;

    FName GetTeamColour() const;

    bool HasTeamFinishedTurn() const;

    void AddUnit(AUnitActor* Unit);
    const TArray<AUnitActor*>& GetControlledUnits() const;

    void ResetUnitsForNewTurn();

    bool OwnsUnit(const AUnitActor* Unit) const;

    bool HasLivingUnits() const;


private:
    UPROPERTY()
    FName TeamColour;

    UPROPERTY()
    bool bPlayerControlled = false;

    UPROPERTY()
    TSubclassOf<AUnitActor> SniperBlueprint;

    UPROPERTY()
    TSubclassOf<AUnitActor> BrawlerBlueprint;

    UPROPERTY()
    TArray<TSubclassOf<AUnitActor>> UnitsLeftToPlace;

    UPROPERTY()
    TArray<AUnitActor*> ControlledUnits;

};