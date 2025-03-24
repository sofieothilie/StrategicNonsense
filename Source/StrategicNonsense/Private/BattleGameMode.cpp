#include "BattleGameMode.h"
#include "GridManager.h"
#include "GridCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Team.h"
#include "SniperUnit.h"
#include "BattlePlayerController.h"

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
    //SpawnTopDownCamera();
    SpawnGridAndSetup();

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

    SetupTeams();
    SpawnTeamUnits();
}

void ABattleGameMode::SetupTeams()
{
    TArray<FName> SelectedColours = { "Blue", "Red", "Violet", "Brown", "Green"};
    for (const FName& Colour : SelectedColours)
    {
        UTeam* Team = NewObject<UTeam>(this);
        Team->Initialise(Colour);
        AllTeams.Add(Team);
    }

}


void ABattleGameMode::SpawnTeamUnits()
{
    if (AllTeams.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No teams to place units for!"));
        return;
    }

    for (UTeam* Team : AllTeams)
    {
        if (!Team) continue;

        TSubclassOf<AUnitActor> SniperClass = Team->GetSniperBlueprint();
        TSubclassOf<AUnitActor> BrawlerClass = Team->GetBrawlerBlueprint();

        if (SniperClass)
        {
            UnitsToPlace.Add(SniperClass);
            UE_LOG(LogTemp, Warning, TEXT("Sniper added to placement queue for team %s"), *Team->GetTeamColour().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Sniper blueprint missing for team %s"), *Team->GetTeamColour().ToString());
        }

        if (BrawlerClass)
        {
            UnitsToPlace.Add(BrawlerClass);
            UE_LOG(LogTemp, Warning, TEXT("Brawler added to placement queue for team %s"), *Team->GetTeamColour().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Brawler blueprint missing for team %s"), *Team->GetTeamColour().ToString());
        }
    }

    CurrentPhase = EGamePhase::Placement;
}



void ABattleGameMode::OnPlayerClickedGrid(const FVector& ClickLocation)
{
    if (CurrentPhase != EGamePhase::Placement || UnitsToPlace.IsEmpty())
        return;

    AGridManager* GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (!GridManager) return;

    TSubclassOf<AUnitActor> UnitClass = UnitsToPlace[0];
    bool bPlaced = GridManager->TryPlaceUnitAtLocation(ClickLocation, UnitClass);

    if (bPlaced)
    {
        UnitsToPlace.RemoveAt(0);

        if (UnitsToPlace.IsEmpty())
        {
            CurrentPhase = EGamePhase::PlayerTurn;
            UE_LOG(LogTemp, Warning, TEXT("All units placed! Game begins."));
        }
    }
}
