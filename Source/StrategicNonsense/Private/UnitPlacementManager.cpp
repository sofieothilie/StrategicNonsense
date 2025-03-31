#include "UnitPlacementManager.h"
#include "BattleGameMode.h"
#include "GridManager.h"
#include "Team.h"
#include "UnitActor.h"
#include "SniperUnit.h"
#include "StartMessageWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "UnitSelectionWidget.h"

/**
 * @brief Initialises the unit placement system with references to the game mode, grid, teams, and starter side.
 *
 * Displays the initial "who starts" message, then begins the alternating unit placement process.
 *
 * @param InGameMode Pointer to the game mode.
 * @param InGridManager Pointer to the grid manager.
 * @param Teams Array containing both teams.
 * @param bPlayerStarts True if the player goes first.
 */
void UUnitPlacementManager::Initialise(ABattleGameMode* InGameMode, AGridManager* InGridManager, const TArray<UTeam*>& Teams, bool bPlayerStarts)
{
    GameMode = InGameMode;
    GridManager = InGridManager;
    AllTeams = Teams;

    CurrentPlacementTeamIndex = bPlayerStarts ? 0 : 1;

    StartText = FText::FromString(
        bPlayerStarts ? TEXT("Player Starts") : TEXT("AI Starts")
    );

    // Show the start message before starting placement
    ShowStartMessage();

    // Delay the placement start to give room for the message
    FTimerHandle PlacementStartHandle;
    GridManager->GetWorld()->GetTimerManager().SetTimer(PlacementStartHandle, [this]()
        {
            GameMode->SetGamePhase(EGamePhase::Placement);
            StartNextPlacementStep();
        }, 2.2f, false);  // Delay slightly longer than the message

    GameMode->SetGamePhase(EGamePhase::Placement);
    StartNextPlacementStep();
}

/**
 * @brief Displays the initial start message widget (e.g. "Player Starts") on the screen.
 */
void UUnitPlacementManager::ShowStartMessage()
{
    FString WidgetPath = TEXT("/Game/Blueprints/WBP_StartMessage.WBP_StartMessage_C");
    TSubclassOf<UStartMessageWidget> StartWidgetClass = Cast<UClass>(StaticLoadClass(UUserWidget::StaticClass(), nullptr, *WidgetPath));
    if (!StartWidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    UStartMessageWidget* StartWidget = CreateWidget<UStartMessageWidget>(PC, StartWidgetClass);
    if (!StartWidget) return;

    StartWidget->SetMessageText(StartText);
    StartWidget->AddToViewport();

    // Optional: hide it automatically
    FTimerHandle TempHandle;
    GridManager->GetWorld()->GetTimerManager().SetTimer(TempHandle, [StartWidget]()
        {
            StartWidget->RemoveFromParent();
        }, 2.0f, false);
}

/**
 * @brief Begins the next step in the unit placement process for the current team.
 *
 * If the player has two units left to choose from, it shows a selection widget.
 * If it's the AI's turn, it randomly selects a unit and delays placement.
 * When all units are placed, it transitions the game to the active phase.
 */
void UUnitPlacementManager::StartNextPlacementStep()
{
    TeamPlacingNext = (CurrentPlacementTeamIndex == 0) ? AllTeams[0] : AllTeams[1];
    TArray<TSubclassOf<AUnitActor>>* AvailableUnits = &TeamPlacingNext->GetUnplacedUnits();

    if (AvailableUnits->IsEmpty())
    {
        // No units left for this team — switch to the other
        CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
        TeamPlacingNext = (CurrentPlacementTeamIndex == 0) ? AllTeams[0] : AllTeams[1];

        // Check the other team
        if (TeamPlacingNext->GetUnplacedUnits().IsEmpty())
        {
            GameMode->SpawnGameStatusWidget();

            GameMode->SetGamePhase(GameMode->DoesPlayerStart() ? EGamePhase::PlayerTurn : EGamePhase::AITurn);
            UE_LOG(LogTemp, Log, TEXT("All units placed. Game begins."));
            return;
        }


        StartNextPlacementStep(); // recursive call for the other team
        return;
    }

    if (TeamPlacingNext->IsPlayerControlled())
    {
        if (AvailableUnits->Num() > 1 && !bUnitSelectionWidgetShown)
        {
            if (!UnitSelectionWidgetClass)
            {
                FString WidgetPath = TEXT("/Game/Blueprints/WBP_UnitSelectionWidget.WBP_UnitSelectionWidget_C");
                UnitSelectionWidgetClass = Cast<UClass>(StaticLoadClass(UUserWidget::StaticClass(), nullptr, *WidgetPath));
                if (!UnitSelectionWidgetClass)
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to load UnitSelectionWidgetClass"));
                    return;
                }
            }

            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (!PC) return;

            UUnitSelectionWidget* Widget = CreateWidget<UUnitSelectionWidget>(PC, UnitSelectionWidgetClass);
            if (!Widget)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create UnitSelectionWidget"));
                return;
            }

            Widget->InitialiseOptions(
                TeamPlacingNext->GetSniperBlueprint(),
                TeamPlacingNext->GetBrawlerBlueprint()
            );

            Widget->OnUnitChosen.AddDynamic(this, &UUnitPlacementManager::HandleUnitChosen);
            Widget->AddToViewport();
            bUnitSelectionWidgetShown = true;
        }

        else
        {
            HandleUnitChosen((*AvailableUnits)[0]);
        }
    }
    else
    {
        int32 Index = FMath::RandRange(0, AvailableUnits->Num() - 1);
        FTimerHandle AIDelayHandle;
        GridManager->GetWorld()->GetTimerManager().SetTimer(AIDelayHandle, [this, AvailableUnits, Index]()
            {
                HandleUnitChosen((*AvailableUnits)[Index]);
            }, 1.0f, false);
    }
}

/**
 * @brief Called when the player (or AI) has chosen which unit to place next.
 * @param ChosenUnit The unit class that was selected for placement.
 */
void UUnitPlacementManager::HandleUnitChosen(TSubclassOf<AUnitActor> ChosenUnit)
{
    UnitToPlaceNext = ChosenUnit;

    UE_LOG(LogTemp, Log, TEXT("%s chose to place %s"),
        *TeamPlacingNext->GetTeamColour().ToString(),
        *GetNameSafe(ChosenUnit));

    if (!TeamPlacingNext->IsPlayerControlled())
    {
        PlaceAIUnit();
    }
}

/**
 * @brief Called when the player clicks on the grid during the placement phase.
 *
 * Places the selected unit at the clicked location if it's valid,
 * then switches to the next team.
 *
 * @param ClickLocation World-space location of the clicked grid cell.
 */
void UUnitPlacementManager::HandlePlayerClickedGrid(const FVector& ClickLocation)
{
    if (!UnitToPlaceNext || !GridManager) return;

    FIntPoint GridCoord = GridManager->WorldToGrid(ClickLocation);
    AUnitActor* NewUnit = GridManager->SpawnAndPlaceUnit(GridCoord, UnitToPlaceNext);
    if (NewUnit)
    {
        TeamPlacingNext->AddUnit(NewUnit);
        RemoveUnitFromQueue(UnitToPlaceNext, TeamPlacingNext);
        UnitToPlaceNext = nullptr;
        CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
        StartNextPlacementStep();
    }
}

/**
 * @brief Randomly places the AI's chosen unit on a valid cell.
 *
 * Once placed, it advances the placement phase to the next team.
 */
void UUnitPlacementManager::PlaceAIUnit()
{
    if (!UnitToPlaceNext || !GridManager) return;

    FVector Location;
    if (GridManager->GetRandomValidPlacementLocation(Location))
    {
        FIntPoint GridCoord = GridManager->WorldToGrid(Location);
        AUnitActor* NewUnit = GridManager->SpawnAndPlaceUnit(GridCoord, UnitToPlaceNext);
        if (NewUnit)
        {
            TeamPlacingNext->AddUnit(NewUnit);

            RemoveUnitFromQueue(UnitToPlaceNext, TeamPlacingNext);
            UnitToPlaceNext = nullptr;
            CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
            StartNextPlacementStep();
        }
    }
}

/**
 * @brief Removes a unit type from the team's placement queue once placed.
 *
 * Ensures that each unit is only placed once per team.
 *
 * @param UnitClass The class of unit to remove.
 * @param OwningTeam The team that owns the unit.
 */
void UUnitPlacementManager::RemoveUnitFromQueue(TSubclassOf<AUnitActor> UnitClass, UTeam* OwningTeam)
{

    TArray<TSubclassOf<AUnitActor>>* UnitsToPlace = &TeamPlacingNext->GetUnplacedUnits();
    for (int32 i = 0; i < UnitsToPlace->Num(); ++i)
    {
        if ((*UnitsToPlace)[i] == UnitClass)
        {
            UnitsToPlace->RemoveAt(i);
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Tried to remove unit %s from team %s, but it wasn't found in queue."),
        *GetNameSafe(UnitClass), *OwningTeam->GetTeamColour().ToString());
}