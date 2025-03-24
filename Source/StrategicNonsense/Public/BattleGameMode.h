#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team.h"
#include "BattleGameMode.generated.h"

class AGridManager;

UCLASS()
class STRATEGICNONSENSE_API ABattleGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABattleGameMode();
    virtual void BeginPlay() override;

private:
    void SpawnTopDownCamera();
    void SpawnGridAndSetup();

    TSubclassOf<AGridManager> GridManagerClass;
    TSubclassOf<AActor> CellBlueprint;
    TSubclassOf<AActor> BP_Tree1;
    TSubclassOf<AActor> BP_Tree2;
    TSubclassOf<AActor> BP_Mountain;

    UPROPERTY()
    TArray<UTeam*> AllTeams;

    void SetupTeams();
    void SpawnTeamUnits();

    AGridManager* SpawnedGridManager;
};
