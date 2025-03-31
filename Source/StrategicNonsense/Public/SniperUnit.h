#pragma once

#include "CoreMinimal.h"
#include "UnitActor.h"
#include "SniperUnit.generated.h"

/**
 * @class ASniperUnit
 * @brief Specialised unit type with distinct movement and attack capabilities.
 *
 * 3 movement, 10 range, ignores obstacles, 4–8 damage, 20 HP.
 */


UCLASS()
class STRATEGICNONSENSE_API ASniperUnit : public AUnitActor
{
    GENERATED_BODY()

public:
    ASniperUnit();

    virtual int32 GetMovementRange() const override { return 3; }

};
