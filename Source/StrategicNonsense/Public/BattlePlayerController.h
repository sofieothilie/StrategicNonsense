#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GridManager.h"
#include "UnitActor.h"
#include "BattlePlayerController.generated.h"

/**
 * @class ABattlePlayerController
 * @brief Manages player input and dispatches actions.
 *
 * Responsible for interpreting mouse clicks, selecting units, and interacting with widgets.
 * Delegates logic to relevant systems like UnitPlacementManager and CombatManager.
 */


class AGridManager;
class AUnitActor;
class UTeam;
class UEndTurnWidget;

UCLASS()
class ABattlePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleEndTurnPressed();

    UFUNCTION()
    void ShowEndTurnWidget();

    UFUNCTION()
    void HideEndTurnWidget();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UEndTurnWidget> EndTurnWidgetClass;

    UPROPERTY()
    UEndTurnWidget* EndTurnWidgetInstance;

protected:
    virtual void SetupInputComponent() override;

private:
    void HandleLeftClick();

    AUnitActor* SelectedUnit = nullptr;
    AGridManager* CachedGridManager = nullptr;

    void HandleUnitClicked(AUnitActor* ClickedUnit);
    void HandleGridCellClicked(FVector ClickLocation);

};
