#include "BattleGameMode.h"
#include "GridManager.h"
#include "GridCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Team.h"
#include "SniperUnit.h"
#include "BattlePlayerController.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/UserWidget.h"
#include "StartMessageWidget.h"
#include "UnitSelectionWidget.h"


ABattleGameMode::ABattleGameMode()
{
    PlayerControllerClass = ABattlePlayerController::StaticClass();

    static ConstructorHelpers::FClassFinder<AActor> CellBP(TEXT("/Game/Blueprints/BP_GridCell"));
    static ConstructorHelpers::FClassFinder<AActor> Tree1BP(TEXT("/Game/Blueprints/BP_Tree1"));
    static ConstructorHelpers::FClassFinder<AActor> Tree2BP(TEXT("/Game/Blueprints/BP_Tree2"));
    static ConstructorHelpers::FClassFinder<AActor> MountainBP(TEXT("/Game/Blueprints/BP_Mountain"));
    FString GridManagerPath = TEXT("/Game/Blueprints/BP_GridManager.BP_GridManager_C");
    GridManagerClass = StaticLoadClass(AGridManager::StaticClass(), nullptr, *GridManagerPath);

    if (!GridManagerClass)
    {
        UE_LOG(LogTemp, Error, TEXT("FAILED to load GridManagerClass from path: %s"), *GridManagerPath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully loaded GridManagerClass!"));
    }

    if (CellBP.Succeeded()) CellBlueprint = CellBP.Class;
    if (Tree1BP.Succeeded()) BP_Tree1 = Tree1BP.Class;
    if (Tree2BP.Succeeded()) BP_Tree2 = Tree2BP.Class;
    if (MountainBP.Succeeded()) BP_Mountain = MountainBP.Class;

}

void ABattleGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTopDownCamera();
    SpawnGridAndSetup();

    DecideStartingPlayer();
    SetupTeams();
    SpawnTeamUnits();
}

void ABattleGameMode::SpawnTopDownCamera()
{
    FVector Location(1000.f, 1000.f, 3000.f);
    FRotator Rotation(-90.f, 0.f, 0.f);
    GetWorld()->SpawnActor<AGridCameraActor>(Location, Rotation);
}

void ABattleGameMode::SpawnGridAndSetup()
{
    FVector Location(0.f, 0.f, 0.f);
    FRotator Rotation(0.f, 0.f, 0.f);

    SpawnedGridManager = GetWorld()->SpawnActor<AGridManager>(GridManagerClass, Location, Rotation);
    if (!SpawnedGridManager) return;

    SpawnedGridManager->SetBlueprints(CellBlueprint, BP_Tree1, BP_Tree2, BP_Mountain);
    SpawnedGridManager->GenerateGrid();
    SpawnedGridManager->PlaceObstacles();
}


void ABattleGameMode::DecideStartingPlayer()
{
    bPlayerStarts = FMath::RandBool();
    FText StartText = FText::FromString(bPlayerStarts ? TEXT("Player Starts") : TEXT("AI Starts"));

    FString WidgetPath = TEXT("/Game/WBP_StartMessage.WBP_StartMessage_C");
    TSubclassOf<UStartMessageWidget> WidgetClass = Cast<UClass>(StaticLoadClass(UStartMessageWidget::StaticClass(), nullptr, *WidgetPath));
    if (!WidgetClass) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    UStartMessageWidget* Widget = CreateWidget<UStartMessageWidget>(PC, WidgetClass);
    if (!Widget) return;

    Widget->SetMessageText(StartText);
    Widget->AddToViewport();
}

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

void ABattleGameMode::SpawnTeamUnits()
{
    UnitsToPlace.Empty();
    for (UTeam* Team : AllTeams)
    {
        if (!Team) continue;

        if (TSubclassOf<AUnitActor> Sniper = Team->GetSniperBlueprint())
        {
            FUnitPlacementEntry Entry;
            Entry.UnitClass = Team->GetSniperBlueprint();
            Entry.OwningTeam = Team;
            UnitsToPlace.Add(Entry);

        }

        if (TSubclassOf<AUnitActor> Brawler = Team->GetBrawlerBlueprint())
        {
            FUnitPlacementEntry Entry;
			Entry.UnitClass = Team->GetBrawlerBlueprint();
            Entry.OwningTeam = Team;
            UnitsToPlace.Add(Entry);
        }
    }

    CurrentPhase = EGamePhase::Placement;
    CurrentPlacementTeamIndex = bPlayerStarts ? 0 : 1;
    PromptNextUnitSelection();
}


void ABattleGameMode::PromptNextUnitSelection()
{
    if (UnitsToPlace.IsEmpty())
    {
        CurrentPhase = EGamePhase::PlayerTurn;
        UE_LOG(LogTemp, Log, TEXT("All units placed. Game begins."));
        return;
    }

    // Alternate index
    TeamPlacingNext = (CurrentPlacementTeamIndex == 0) ? Team1 : Team2;

    // Determine what units are available for this team
    TArray<TSubclassOf<AUnitActor>> Available;
    for (const FUnitPlacementEntry& Entry : UnitsToPlace)
    {
        if (Entry.OwningTeam == TeamPlacingNext)
        {
            Available.Add(Entry.UnitClass);
        }
    }

    if (Available.Num() == 0)
    {
        CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
        PromptNextUnitSelection();
        return;
    }

    if (TeamPlacingNext->IsPlayerControlled())
    {
        if (Available.Num() > 1 && !bUnitSelectionWidgetShown)
        {
            ShowUnitSelection(TeamPlacingNext);
            bUnitSelectionWidgetShown = true;
        }
        else
        {
            HandleUnitChosen(Available[0]);
        }
    }

    else
    {
        int32 Index = FMath::RandRange(0, Available.Num() - 1);
        FTimerHandle AIDelayHandle;
        GetWorld()->GetTimerManager().SetTimer(AIDelayHandle, [this, Available, Index]()
            {
                HandleUnitChosen(Available[Index]);
            }, 1.0f, false); // 1.0 seconds delay

    }
}




void ABattleGameMode::HandleUnitChosen(TSubclassOf<AUnitActor> ChosenUnit)
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


void ABattleGameMode::OnPlayerClickedGrid(const FVector& ClickLocation)
{
    if (CurrentPhase != EGamePhase::Placement || !UnitToPlaceNext) return;

    AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (!GridManager) return;

    if (GridManager->TryPlaceUnitAtLocation(ClickLocation, UnitToPlaceNext))
    {
        // Remove only one matching unit from the correct team
        for (int32 i = 0; i < UnitsToPlace.Num(); ++i)
        {
            if (UnitsToPlace[i].UnitClass == UnitToPlaceNext && UnitsToPlace[i].OwningTeam == TeamPlacingNext)
            {
                UnitsToPlace.RemoveAt(i);
                break;
            }
        }

        UnitToPlaceNext = nullptr;
        CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
        PromptNextUnitSelection();
    }
}

void ABattleGameMode::ShowUnitSelection(UTeam* CurrentTeam)
{
    if (!CurrentTeam)
    {
        UE_LOG(LogTemp, Error, TEXT("CurrentTeam is null!"));
        return;
    }

    // Statically load widget class (only once)
    if (!UnitSelectionWidgetClass)
    {
        FString WidgetPath = TEXT("/Game/Blueprints/WBP_UnitSelectionWidget.WBP_UnitSelectionWidget_C");
        UnitSelectionWidgetClass = Cast<UClass>(StaticLoadClass(UUserWidget::StaticClass(), nullptr, *WidgetPath));

        if (!UnitSelectionWidgetClass)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to statically load UnitSelectionWidgetClass from: %s"), *WidgetPath);
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("Successfully loaded UnitSelectionWidgetClass"));
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("No player controller found!"));
        return;
    }

    UUnitSelectionWidget* Widget = CreateWidget<UUnitSelectionWidget>(PC, UnitSelectionWidgetClass);
    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UnitSelectionWidget"));
        return;
    }

    Widget->InitialiseOptions(CurrentTeam->GetSniperBlueprint(), CurrentTeam->GetBrawlerBlueprint());
    Widget->OnUnitChosen.AddDynamic(this, &ABattleGameMode::HandleUnitChosen);
    Widget->AddToViewport();
}


void ABattleGameMode::PlaceAIUnit()
{
    if (!UnitToPlaceNext || !SpawnedGridManager) return;

    FVector Location;
    if (SpawnedGridManager->GetRandomValidPlacementLocation(Location))
    {
        if (SpawnedGridManager->TryPlaceUnitAtLocation(Location, UnitToPlaceNext))
        {
            // Remove the unit from queue
            for (int32 i = 0; i < UnitsToPlace.Num(); ++i)
            {
                if (UnitsToPlace[i].UnitClass == UnitToPlaceNext && UnitsToPlace[i].OwningTeam == TeamPlacingNext)
                {
                    UnitsToPlace.RemoveAt(i);
                    break;
                }
            }

            UnitToPlaceNext = nullptr;
            CurrentPlacementTeamIndex = 1 - CurrentPlacementTeamIndex;
            PromptNextUnitSelection();
        }
    }
}
