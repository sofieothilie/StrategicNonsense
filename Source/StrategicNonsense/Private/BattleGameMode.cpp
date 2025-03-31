#include "BattleGameMode.h"
#include "GridManager.h"
#include "GridCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Team.h"
#include "SniperUnit.h"
#include "UnitPlacementManager.h"
#include "BattlePlayerController.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/UserWidget.h"
#include "StartMessageWidget.h"
#include "UnitSelectionWidget.h"
#include "GameStatusWidget.h"
#include "EndTurnWidget.h"
#include "GameOverWidget.h"
#include "CombatManager.h"


/**
 * @brief Constructor that sets up the player controller class and loads the GameOver widget class.
 */
ABattleGameMode::ABattleGameMode()
{
    PlayerControllerClass = ABattlePlayerController::StaticClass();

    FString WidgetPath = TEXT("/Game//Blueprints/WBP_GameOver.WBP_GameOver_C");
    TSubclassOf<UGameOverWidget> GameOverWidgetClassLoaded = Cast<UClass>(StaticLoadClass(UUserWidget::StaticClass(), nullptr, *WidgetPath));

    if (GameOverWidgetClassLoaded)
    {
        GameOverWidgetClass = GameOverWidgetClassLoaded;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load GameOverWidget class!"));
    }

}

/**
 * @brief Called when the game begins. Spawns the camera and grid, initialises combat and unit placement managers, and starts team setup.
 */
void ABattleGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTopDownCamera();
    SpawnGridAndSetup();

    CombatManager = NewObject<UCombatManager>(this);
    CombatManager->Initialise(SpawnedGridManager);


    DecideStartingPlayer();
    SetupTeams();
    UnitPlacementManager = NewObject<UUnitPlacementManager>(this);
    UnitPlacementManager->Initialise(this, SpawnedGridManager, AllTeams, bPlayerStarts);
}

/**
 * @brief Spawns a fixed top-down camera above the grid.
 */
void ABattleGameMode::SpawnTopDownCamera()
{
    FVector Location(1000.f, 1000.f, 3000.f);
    FRotator Rotation(-90.f, 0.f, 0.f);
    GetWorld()->SpawnActor<AGridCameraActor>(Location, Rotation);
}

/**
 * @brief Spawns the grid manager, generates the grid, and places obstacles.
 */
void ABattleGameMode::SpawnGridAndSetup()

{
    FVector Location(0.f, 0.f, 0.f);
    FRotator Rotation(0.f, 0.f, 0.f);

    SpawnedGridManager = GetWorld()->SpawnActor<AGridManager>(Location, Rotation);
    if (!SpawnedGridManager) return;

    SpawnedGridManager->GenerateGrid();
    SpawnedGridManager->PlaceObstacles();
}

/**
 * @brief Randomly determines whether the player or AI will start.
 */
void ABattleGameMode::DecideStartingPlayer()
{
    bPlayerStarts = FMath::RandBool();
}

/**
 * @brief Randomly selects team colours and initialises both teams (player and AI).
 */
void ABattleGameMode::SetupTeams()
{
    TArray<FName> AvailableColours = { "Blue", "Red", "Violet", "Brown", "Green" };

    for (int32 i = AvailableColours.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        AvailableColours.Swap(i, j);
    }

    FName Colour1 = AvailableColours[0];
    FName Colour2 = AvailableColours[1];

    Team1 = NewObject<UTeam>(this);
    Team1->Initialise(Colour1, true);

    Team2 = NewObject<UTeam>(this);
    Team2->Initialise(Colour2, false);

    AllTeams.Add(Team1);
    AllTeams.Add(Team2);

    UE_LOG(LogTemp, Warning, TEXT("Selected Teams: %s for player and %s for AI"), *Colour1.ToString(), *Colour2.ToString());
}

/**
 * @brief Handles when the player clicks on the grid during the placement phase.
 * @param ClickLocation The world-space location that was clicked.
 */
void ABattleGameMode::OnPlayerClickedGrid(const FVector& ClickLocation)
{

    if (CurrentPhase == EGamePhase::Placement && UnitPlacementManager)
    {
        UnitPlacementManager->HandlePlayerClickedGrid(ClickLocation);
    }
}

/**
 * @brief Sets the current game phase and triggers logic associated with each phase (e.g., reset units, show/hide widgets).
 * @param NewPhase The new phase of the game (e.g., PlayerTurn, AITurn).
 */
void ABattleGameMode::SetGamePhase(EGamePhase NewPhase)
{
    CurrentPhase = NewPhase;

    UTeam* PlayerTeam = GetPlayerTeam();
    UTeam* AITeam = (PlayerTeam == Team1) ? Team2 : Team1;

    ABattlePlayerController* PlayerController = Cast<ABattlePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    switch (NewPhase)
    {
    case EGamePhase::PlayerTurn:
        if (PlayerTeam)
        {
            PlayerTeam->ResetUnitsForNewTurn();
            UE_LOG(LogTemp, Warning, TEXT("Player units reset for new turn."));
        }

        if (PlayerController)
        {
            // Load and show EndTurn widget
            if (!PlayerController->EndTurnWidgetClass)
            {
                FString WidgetPath = TEXT("/Game/Blueprints/WBP_EndTurnWidget.WBP_EndTurnWidget_C");
                TSubclassOf<UEndTurnWidget> LoadedClass = Cast<UClass>(StaticLoadClass(UUserWidget::StaticClass(), nullptr, *WidgetPath));
                if (LoadedClass)
                {
                    PlayerController->EndTurnWidgetClass = LoadedClass;
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to load EndTurnWidget class!"));
                }
            }

            PlayerController->ShowEndTurnWidget();
        }
        break;

    case EGamePhase::AITurn:
        if (AITeam)
        {
            AITeam->ResetUnitsForNewTurn();
            UE_LOG(LogTemp, Warning, TEXT("AI units reset for new turn."));

            FTimerHandle AITimer;
            GetWorld()->GetTimerManager().SetTimer(AITimer, this, &ABattleGameMode::HandleAITurn, 0.6f, false);
        }

        if (PlayerController)
        {
            PlayerController->HideEndTurnWidget();
        }
        break;

    default:
        break;
    }

    if (GameStatusWidget)
    {
        const FString TurnText = (NewPhase == EGamePhase::PlayerTurn) ? TEXT("Player Turn") : TEXT("AI Turn");
        GameStatusWidget->SetTurnText(TurnText);
    }
}


/**
 * @brief Retrieves the team controlled by the player.
 * @return Pointer to the player-controlled team.
 */
UTeam* ABattleGameMode::GetPlayerTeam() const
{ 
    return Team1->IsPlayerControlled() ? Team1 : Team2;
}

/**
 * @brief Retrieves the team controlled by the AI.
 * @return Pointer to the AI-controlled team.
 */
UTeam* ABattleGameMode::GetAITeam() const
{
    return Team1->IsPlayerControlled() ? Team2 : Team1;
}

/**
 * @brief Executes the AI turn logic: moves units randomly and performs attacks if in range.
 * Ends the turn and transitions back to the player.
 */
void ABattleGameMode::HandleAITurn()
{
    UTeam* AITeam = GetAITeam();
    if (!AITeam || !SpawnedGridManager || !CombatManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot run AI turn - missing team, grid, or combat manager."));
        return;
    }

    TArray<AUnitActor*> Units = AITeam->GetControlledUnits();

    for (AUnitActor* Unit : Units)
    {
        if (!Unit || Unit->HasMovedThisTurn())
            continue;

        // Random movement
        FIntPoint Current = Unit->GetGridPosition();
        int32 Range = Unit->GetMovementRange();
        TSet<FIntPoint> Reachable = SpawnedGridManager->FindReachableCellsBFS(Current, Range);

        if (Reachable.Num() == 0)
            continue;

        TArray<FIntPoint> ReachableArray = Reachable.Array();
        int32 RandIndex = FMath::RandRange(0, ReachableArray.Num() - 1);
        FIntPoint Destination = ReachableArray[RandIndex];

        SpawnedGridManager->SetUnitAtCell(Current, nullptr);
        SpawnedGridManager->SetUnitAtCell(Destination, Unit);
        Unit->SetGridPosition(Destination);
        Unit->SetActorLocation(SpawnedGridManager->GridToWorld(Destination));
        Unit->MarkAsMoved();

        UE_LOG(LogTemp, Warning, TEXT("AI moved %s to (%d, %d)"), *Unit->GetName(), Destination.X, Destination.Y);

        // Try to attack after moving
        TArray<AUnitActor*> EnemyUnits = GetPlayerTeam()->GetControlledUnits();
        for (AUnitActor* Target : EnemyUnits)
        {
            if (!Target || Target->IsDead()) continue;

            FIntPoint From = Unit->GetGridPosition();
            FIntPoint To = Target->GetGridPosition();
            int32 Distance = FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);

            if (Distance <= Unit->GetAttackRange())
            {
                if (CombatManager->ExecuteAttack(Unit, Target))
                {
                    CheckGameEnd(); 
                }

                break; // Attack once per turn
            }
        }
    }

    // End AI turn and go back to player
    SetGamePhase(EGamePhase::PlayerTurn);
}


/**
 * @brief Checks whether either team has lost all units and triggers game-over logic if so.
 */
void ABattleGameMode::CheckGameEnd()
{
    if (!Team1 || !Team2 || CurrentPhase == EGamePhase::GameOver)
        return;

    UTeam* PlayerTeam = GetPlayerTeam();
    UTeam* AITeam = GetAITeam();

    const bool bPlayerAlive = PlayerTeam->HasLivingUnits();
    const bool bAIAlive = AITeam->HasLivingUnits();

    if (!bPlayerAlive && !bAIAlive)
    {
        UE_LOG(LogTemp, Warning, TEXT("It's a draw!"));
        CurrentPhase = EGamePhase::GameOver;
        ShowGameOverWidget(TEXT("Draw!"));
    }
    else if (!bPlayerAlive)
    {
        FString Winner = AITeam->GetTeamColour().ToString();
        UE_LOG(LogTemp, Warning, TEXT("%s wins!"), *Winner);
        CurrentPhase = EGamePhase::GameOver;
        ShowGameOverWidget(FString::Printf(TEXT("%s wins!"), *Winner));
    }
    else if (!bAIAlive)
    {
        FString Winner = PlayerTeam->GetTeamColour().ToString();
        UE_LOG(LogTemp, Warning, TEXT("%s wins!"), *Winner);
        CurrentPhase = EGamePhase::GameOver;
        ShowGameOverWidget(FString::Printf(TEXT("%s wins!"), *Winner));
    }
}


/**
 * @brief Displays the game over widget with the given result text (e.g., who won).
 * @param ResultText The text to display in the widget.
 */
void ABattleGameMode::ShowGameOverWidget(const FString& ResultText)
{
    if (!GameOverWidgetClass) return;

    GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
    if (!GameOverWidget) return;

    GameOverWidget->AddToViewport();

    if (UGameOverWidget* Casted = Cast<UGameOverWidget>(GameOverWidget))
    {
        Casted->SetResultText(ResultText);
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->SetPause(true);
    }
}

/**
 * @brief Loads and displays the GameStatus widget, setting initial team and unit health information.
 */
void ABattleGameMode::SpawnGameStatusWidget()
{
    FString WidgetPath = TEXT("/Game/Blueprints/WBP_GameStatus.WBP_GameStatus_C");
    TSubclassOf<UGameStatusWidget> GameStatusWidgetClassLoaded =
        Cast<UClass>(StaticLoadClass(UGameStatusWidget::StaticClass(), nullptr, *WidgetPath));


    if (!GameStatusWidgetClassLoaded)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load GameStatusWidget class!"));
        return;
    }

    UGameStatusWidget* Widget = CreateWidget<UGameStatusWidget>(GetWorld(), GameStatusWidgetClassLoaded);
    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create GameStatusWidget!"));
        return;
    }

    Widget->AddToViewport();
    GameStatusWidget = Widget;

    // Set initial status
    if (Team1 && Team2)
    {
        Widget->SetTeamInfo(Team1->IsPlayerControlled(), Team1->GetTeamColour().ToString(), !Team1->IsPlayerControlled());
        Widget->SetTeamInfo(!Team1->IsPlayerControlled(), Team2->GetTeamColour().ToString(), !Team2->IsPlayerControlled());

        for (AUnitActor* Unit : Team1->GetControlledUnits())
        {
            Widget->SetUnitHealth(Team1->IsPlayerControlled(), Unit->GetUnitType(), Unit->GetHealth(), /* MaxHP */ Unit->GetHealth());
        }

        for (AUnitActor* Unit : Team2->GetControlledUnits())
        {
            Widget->SetUnitHealth(Team2->IsPlayerControlled(), Unit->GetUnitType(), Unit->GetHealth(), /* MaxHP */ Unit->GetHealth());
        }
    }
}

/**
 * @brief Updates the Game Status Widget with current unit health for both teams.
 * Displays 0 HP for units that are dead or missing.
 */
void ABattleGameMode::UpdateGameStatusWidget()
{
    if (!GameStatusWidget || !Team1 || !Team2)
        return;

    auto UpdateTeam = [&](UTeam* Team)
        {
            bool IsPlayer = Team->IsPlayerControlled();
            AUnitActor* Sniper = nullptr;
            AUnitActor* Brawler = nullptr;

            for (AUnitActor* Unit : Team->GetControlledUnits())
            {
                if (!Unit) continue;

                if (Unit->GetUnitType() == EGameUnitType::Sniper && !Sniper && !Unit->IsDead())
                    Sniper = Unit;

                if (Unit->GetUnitType() == EGameUnitType::Brawler && !Brawler && !Unit->IsDead())
                    Brawler = Unit;
            }

            // Sniper
            if (Sniper)
            {
                GameStatusWidget->SetUnitHealth(IsPlayer, EGameUnitType::Sniper, Sniper->GetHealth(), Sniper->GetMaxHealth());
            }
            else
            {
                GameStatusWidget->SetUnitHealth(IsPlayer, EGameUnitType::Sniper, 0, 20); // Max for sniper
            }

            // Brawler
            if (Brawler)
            {
                GameStatusWidget->SetUnitHealth(IsPlayer, EGameUnitType::Brawler, Brawler->GetHealth(), Brawler->GetMaxHealth());
            }
            else
            {
                GameStatusWidget->SetUnitHealth(IsPlayer, EGameUnitType::Brawler, 0, 40); // Max for brawler
            }
        };

    UpdateTeam(Team1);
    UpdateTeam(Team2);
}