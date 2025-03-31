#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatManager.generated.h"


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
