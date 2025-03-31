#pragma once

#include "CoreMinimal.h"
#include "UnitActor.h"
#include "BrawlerUnit.generated.h"

/**
 * @class  ABrawlerUnit
 * @brief Specialised unit type with distinct movement and attack capabilities.
 *
 * - Brawler: 6 movement, 1 range, 1–6 damage, 40 HP.
 */


UCLASS()
class STRATEGICNONSENSE_API ABrawlerUnit : public AUnitActor
{
    GENERATED_BODY()

public:
    ABrawlerUnit();

    virtual int32 GetMovementRange() const override { return 6; }

};
