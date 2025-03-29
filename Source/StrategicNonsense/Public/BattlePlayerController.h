#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GridManager.h"
#include "UnitActor.h"
#include "BattlePlayerController.generated.h"


class AGridManager;
class AUnitActor;
class UTeam;

UCLASS()
class ABattlePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    virtual void SetupInputComponent() override;

private:
    void HandleLeftClick();

    AUnitActor* SelectedUnit = nullptr;
    AGridManager* CachedGridManager = nullptr;

    void HandleUnitClicked(AUnitActor* ClickedUnit);
    void HandleGridCellClicked(FVector ClickLocation);
};
