#include "BattleGameMode.h"
#include "GridManager.h"
#include "GridCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Team.h"
#include "SniperUnit.h"

ABattleGameMode::ABattleGameMode()
{
    UE_LOG(LogTemp, Warning, TEXT("ABattleGameMode constructor called"));

    static ConstructorHelpers::FClassFinder<AActor> CellBP(TEXT("/Game/Blueprints/BP_GridCell"));
    static ConstructorHelpers::FClassFinder<AActor> Tree1BP(TEXT("/Game/Blueprints/BP_Tree1"));
    static ConstructorHelpers::FClassFinder<AActor> Tree2BP(TEXT("/Game/Blueprints/BP_Tree2"));
    static ConstructorHelpers::FClassFinder<AActor> MountainBP(TEXT("/Game/Blueprints/BP_Mountain"));
    static ConstructorHelpers::FClassFinder<AGridManager> GridBP(TEXT("/Game/Blueprints/BP_GridManager"));

    if (CellBP.Succeeded()) CellBlueprint = CellBP.Class;
    if (Tree1BP.Succeeded()) BP_Tree1 = Tree1BP.Class;
    if (Tree2BP.Succeeded()) BP_Tree2 = Tree2BP.Class;
    if (MountainBP.Succeeded()) BP_Mountain = MountainBP.Class;
    if (GridBP.Succeeded()) GridManagerClass = GridBP.Class;
}

void ABattleGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTopDownCamera();
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
    if (!SpawnedGridManager) return;

    float GroundHeightOffset = 50.0f;
    int32 StartX = 2;
    int32 StartY = 2;

    for (UTeam* Team : AllTeams)
    {
        if (!Team) continue;

        FName Colour = Team->GetTeamColour();
        UE_LOG(LogTemp, Warning, TEXT("Team colour: %s"), *Colour.ToString());

        FVector SniperLocation((StartX + 0.5f) * 100.f, (StartY + 0.5f) * 100.f, GroundHeightOffset);
        FVector BrawlerLocation((StartX + 1.5f) * 100.f, (StartY + 0.5f) * 100.f, GroundHeightOffset);
        float CellSize = SpawnedGridManager->GetCellSize();
        FRotator rotation = FRotator(0.f, 0.f, 90.f);
        
        TSubclassOf<AUnitActor> SniperClass = Team->GetSniperBlueprint();
        if (!SniperClass)
        {
            UE_LOG(LogTemp, Error, TEXT("SniperBlueprint is null for team %s"), *Colour.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawning Sniper for team %s..."), *Colour.ToString());
            AActor* Sniper = GetWorld()->SpawnActor<AActor>(SniperClass, SniperLocation, FRotator::ZeroRotator);
            if (Sniper)
            {
                Sniper->SetActorRotation(rotation);
                
                float PaddingFactor = 0.1f;
                float UnitScale = (CellSize / 100.f) * PaddingFactor;
                Sniper->SetActorScale3D(FVector(UnitScale));


                Sniper->SetFolderPath(FName("Units"));
                UE_LOG(LogTemp, Warning, TEXT("Sniper spawned and rotated for team %s!"), *Colour.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("FAILED to spawn Sniper for team %s!"), *Colour.ToString());
            }
        }

        TSubclassOf<AUnitActor> BrawlerClass = Team->GetBrawlerBlueprint();
        if (!BrawlerClass)
        {
            UE_LOG(LogTemp, Error, TEXT("BrawlerBlueprint is null for team %s"), *Colour.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawning Brawler for team %s..."), *Colour.ToString());
            AActor* Brawler = GetWorld()->SpawnActor<AActor>(BrawlerClass, BrawlerLocation, FRotator::ZeroRotator);
            if (Brawler)
            {
                Brawler->SetActorRotation(rotation);
                float PaddingFactor = 0.15f;
                float UnitScale = (CellSize / 100.f) * PaddingFactor;
                Brawler->SetActorScale3D(FVector(UnitScale));
                Brawler->SetFolderPath(FName("Units"));
                UE_LOG(LogTemp, Warning, TEXT("Brawler spawned and rotated for team %s!"), *Colour.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("FAILED to spawn Brawler for team %s!"), *Colour.ToString());
            }
        }

        StartX += 3;
    }
}

