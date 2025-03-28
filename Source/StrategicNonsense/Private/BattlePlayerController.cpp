#include "BattlePlayerController.h"
#include "BattleGameMode.h"
#include "GridManager.h"
#include "Team.h"
#include "UnitActor.h"
#include "Kismet/GameplayStatics.h"

void ABattlePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAction("LeftClick", IE_Pressed, this, &ABattlePlayerController::HandleLeftClick);
}

void ABattlePlayerController::HandleLeftClick()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (!Hit.bBlockingHit)
        return;

    ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GameMode)
        return;

    // Branch based on game phase
    switch (GameMode->GetCurrentPhase())
    {
    case EGamePhase::Placement:
        UE_LOG(LogTemp, Warning, TEXT("Placement phase - forwarding to game mode"));
        GameMode->OnPlayerClickedGrid(Hit.Location);
        break;

    case EGamePhase::PlayerTurn:
        if (!CachedGridManager)
        {
            TArray<AActor*> FoundManagers;
            UGameplayStatics::GetAllActorsOfClass(this, AGridManager::StaticClass(), FoundManagers);
            if (FoundManagers.Num() > 0)
            {
                CachedGridManager = Cast<AGridManager>(FoundManagers[0]);
            }
        }

        // Check what was clicked
        if (AUnitActor* ClickedUnit = Cast<AUnitActor>(Hit.GetActor()))
        {
            HandleUnitClicked(ClickedUnit);
        }
        else
        {
            HandleGridCellClicked(Hit.Location);
        }
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Click ignored - not in an interactive phase."));
        break;
    }
}


void ABattlePlayerController::HandleUnitClicked(AUnitActor* ClickedUnit)
{
    if (!ClickedUnit) return;

    ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GameMode) return;

    // If we already selected a unit, try attacking
    if (SelectedUnit && SelectedUnit != ClickedUnit)
    {
        if (GameMode->GetPlayerTeam()->OwnsUnit(SelectedUnit) &&
            !GameMode->GetPlayerTeam()->OwnsUnit(ClickedUnit) &&
            GameMode->CombatManager)
        {
            bool bSuccess = GameMode->CombatManager->ExecuteAttack(SelectedUnit, ClickedUnit);

            if (bSuccess)
            {
                SelectedUnit = nullptr;

                if (GameMode->GetPlayerTeam()->HasTeamFinishedTurn())
                {
                    GameMode->SetGamePhase(EGamePhase::AITurn);
                }
            }
            return;
        }
    }

    // Selecting a new friendly unit
    if (GameMode->GetPlayerTeam()->OwnsUnit(ClickedUnit))
    {
        SelectedUnit = ClickedUnit;
        UE_LOG(LogTemp, Warning, TEXT("Selected Unit: %s"), *ClickedUnit->GetName());
    }
}


void ABattlePlayerController::HandleGridCellClicked(FVector ClickLocation)
{
    if (!SelectedUnit || !CachedGridManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing unit or grid manager."));
        return;
    }

    if (SelectedUnit->HasMovedThisTurn())
    {
        UE_LOG(LogTemp, Warning, TEXT("Unit has already moved this turn."));
        return;
    }

    FIntPoint TargetCell = CachedGridManager->WorldToGrid(ClickLocation);
    FIntPoint CurrentCell = SelectedUnit->GetGridPosition();

    if (TargetCell == CurrentCell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Clicked on current unit position."));
        return;
    }

    int32 MaxRange = SelectedUnit->GetMovementRange();
    TSet<FIntPoint> ReachableCells = CachedGridManager->FindReachableCellsBFS(CurrentCell, MaxRange);

    if (!ReachableCells.Contains(TargetCell))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cell (%d, %d) is outside movement range."), TargetCell.X, TargetCell.Y);
        return;
    }

    if (!CachedGridManager->IsCellWalkable(TargetCell))
    {
        UE_LOG(LogTemp, Warning, TEXT("Target cell is not walkable."));
        return;
    }

    // Move
    UE_LOG(LogTemp, Warning, TEXT("Snapping unit to cell (%d, %d)"), TargetCell.X, TargetCell.Y);
    CachedGridManager->SetUnitAtCell(CurrentCell, nullptr);
    CachedGridManager->SetUnitAtCell(TargetCell, SelectedUnit);
    SelectedUnit->SetGridPosition(TargetCell);
    SelectedUnit->SetActorLocation(CachedGridManager->GridToWorld(TargetCell));
    SelectedUnit->MarkAsMoved();

    // Check if player turn is over
    ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        UTeam* PlayerTeam = GameMode->GetPlayerTeam();
        if (PlayerTeam && PlayerTeam->HasTeamFinishedTurn())
        {
            UE_LOG(LogTemp, Warning, TEXT("Player has finished their turn."));
            GameMode->SetGamePhase(EGamePhase::AITurn);
        }
    }

    SelectedUnit = nullptr;
}
