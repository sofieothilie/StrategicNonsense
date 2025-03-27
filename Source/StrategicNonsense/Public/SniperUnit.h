#pragma once

#include "CoreMinimal.h"
#include "UnitActor.h"
#include "SniperUnit.generated.h"

UCLASS()
class STRATEGICNONSENSE_API ASniperUnit : public AUnitActor
{
    GENERATED_BODY()

public:
    ASniperUnit();

    virtual int32 GetMovementRange() const override { return 3; }

};
