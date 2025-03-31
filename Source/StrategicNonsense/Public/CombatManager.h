#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatManager.generated.h"

/**
 * @class ACombatManager
 * @brief Handles combat interactions between units.
 *
 * Coordinates attack actions, calculates and applies damage,
 * and processes counterattack logic when applicable.
 * Integrates with unit state and grid visuals.
 */



class AUnitActor;
class AGridManager;
class ABattleGameMode;


UCLASS()
class STRATEGICNONSENSE_API UCombatManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialise(AGridManager* Grid);
    bool ExecuteAttack(AUnitActor* Attacker, AUnitActor* Target);

    UPROPERTY()
    ABattleGameMode* GameMode;


private:
    AGridManager* GridManager;

    bool IsInRange(AUnitActor* Attacker, AUnitActor* Target) const;
    void HandleCounterattack(AUnitActor* Attacker, AUnitActor* Target);
    void RemoveUnitIfDead(AUnitActor* Unit);
};
