#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Team.generated.h"

class AUnitActor;

UCLASS(Blueprintable)
class STRATEGICNONSENSE_API UTeam : public UObject
{
    GENERATED_BODY()

public:
    void Initialise(FName InTeamColour);

    FName GetTeamColour() const;
    TSubclassOf<AUnitActor> GetSniperBlueprint() const;
    TSubclassOf<AUnitActor> GetBrawlerBlueprint() const;

private:
    UPROPERTY()
    FName TeamColour;

    UPROPERTY()
    TSubclassOf<AUnitActor> SniperBlueprint;

    UPROPERTY()
    TSubclassOf<AUnitActor> BrawlerBlueprint;
};