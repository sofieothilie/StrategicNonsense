#include "GridManager.h"
#include "SniperUnit.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "DrawDebugHelpers.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
    SetBlueprints();
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
}
void AGridManager::GenerateGrid()
{
    if (!CellBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("CellBlueprint is null"));
        return;
    }

    float GroundHeightOffset = 0.0f;

    for (int32 X = 0; X < GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < GridSizeY; ++Y)
        {
            FVector Location = FVector(
                (X + 0.5f) * CellSize,
                (Y + 0.5f) * CellSize,
                GroundHeightOffset
            );

            AActor* NewCell = GetWorld()->SpawnActor<AActor>(CellBlueprint, Location, FRotator::ZeroRotator);

            if (NewCell)
            {
                NewCell->SetFolderPath(FName("Grid"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn cell at X=%d Y=%d"), X, Y);
            }
        }
    }
}

void AGridManager::SetBlueprints()
{
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



void AGridManager::PlaceObstacles()
{
    if (!BP_Mountain && !BP_Tree1 && !BP_Tree2) return;

    int32 TotalCells = GridSizeX * GridSizeY;
    int32 NumObstacles = FMath::RoundToInt(TotalCells * (ObstaclePercentage / 100.0f));

    TArray<TSubclassOf<AActor>> ObstacleTypes = { BP_Mountain, BP_Tree1, BP_Tree2 };

    TMap<TSubclassOf<AActor>, FIntPoint> ObstacleSizes;
    ObstacleSizes.Add(BP_Mountain, FIntPoint(6, 9));
    ObstacleSizes.Add(BP_Tree1, FIntPoint(1, 1));
    ObstacleSizes.Add(BP_Tree2, FIntPoint(1, 1));

    float GroundHeightOffset = 50.0f;

    while (OccupiedCells.Num() < NumObstacles)
    {
        TSubclassOf<AActor> ChosenObstacle = ObstacleTypes[FMath::RandRange(0, ObstacleTypes.Num() - 1)];
        FIntPoint ObstacleSize = ObstacleSizes[ChosenObstacle];

        int32 X = FMath::RandRange(0, GridSizeX - ObstacleSize.X - 1);
        int32 Y = FMath::RandRange(0, GridSizeY - ObstacleSize.Y - 1);

        bool CanPlace = true;
        TArray<FIntPoint> CellsToOccupy;

        for (int32 i = 0; i < ObstacleSize.X; ++i)
        {
            for (int32 j = 0; j < ObstacleSize.Y; ++j)
            {
                FIntPoint TestCell(X + i, Y + j);
                if (OccupiedCells.Contains(TestCell))
                {
                    CanPlace = false;
                    break;
                }
                CellsToOccupy.Add(TestCell);
            }
            if (!CanPlace) break;
        }

        if (!CanPlace) continue;

        FVector SpawnLocation;

        if (ObstacleSize == FIntPoint(1, 1))
        {
            SpawnLocation = FVector(
                (X + 0.5f) * CellSize,
                (Y + 0.5f) * CellSize,
                GroundHeightOffset
            );
        }
        else
        {
            SpawnLocation = FVector(
                (X + ObstacleSize.X / 2.0f) * CellSize,
                (Y + ObstacleSize.Y / 2.0f) * CellSize,
                GroundHeightOffset
            );
        }

        FRotator SpawnRotation = FRotator(0, 0, 90);
        AActor* SpawnedObstacle = GetWorld()->SpawnActor<AActor>(ChosenObstacle, SpawnLocation, SpawnRotation);

        if (SpawnedObstacle)
        {
            SpawnedObstacle->SetFolderPath(FName("Obstacle"));

            for (const FIntPoint& Cell : CellsToOccupy)
            {
                OccupiedCells.Add(Cell);
            }

            UE_LOG(LogTemp, Warning, TEXT("Spawned obstacle at Grid (%d, %d) -> World (%f, %f)"),
                X, Y, SpawnLocation.X, SpawnLocation.Y);
        }
    }
}

bool AGridManager::TryPlaceUnitAtLocation(const FVector& ClickLocation, TSubclassOf<AUnitActor> UnitToPlace)
{
    FIntPoint GridCoord = WorldToGrid(ClickLocation);
    if (!IsCellValid(GridCoord) || OccupiedCells.Contains(GridCoord))
        return false;

    FVector SpawnLocation = GridToWorld(GridCoord);
    FRotator Rotation = FRotator(0.f, 0.f, 90.f);

    AUnitActor* NewUnit = GetWorld()->SpawnActor<AUnitActor>(UnitToPlace, SpawnLocation, Rotation);
    if (!NewUnit)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn unit at (%d, %d)"), GridCoord.X, GridCoord.Y);
        return false;
    }

    // Mark the cell as used
    OccupiedCells.Add(GridCoord);

    // Scale the unit based on cell size
    float PaddingFactor = NewUnit->IsA(ASniperUnit::StaticClass()) ? 0.1f : 0.15f;
    float UnitScale = (CellSize / 100.f) * PaddingFactor;
    NewUnit->SetActorScale3D(FVector(UnitScale));

    // Folder for organisation
    NewUnit->SetFolderPath(FName("Units"));

    UE_LOG(LogTemp, Warning, TEXT("Spawned and scaled %s at (%d, %d)"), *NewUnit->GetName(), GridCoord.X, GridCoord.Y);
    return true;
}


bool AGridManager::IsCellValid(const FIntPoint& Cell) const
{
    return Cell.X >= 0 && Cell.X < GridSizeX && Cell.Y >= 0 && Cell.Y < GridSizeY;
}

FIntPoint AGridManager::WorldToGrid(const FVector& Location) const
{
    int32 X = FMath::FloorToInt(Location.X / CellSize);
    int32 Y = FMath::FloorToInt(Location.Y / CellSize);
    return FIntPoint(X, Y);
}

FVector AGridManager::GridToWorld(const FIntPoint& Cell) const
{
    return FVector(
        (Cell.X + 0.5f) * CellSize, // x
        (Cell.Y + 0.5f) * CellSize, // y
        50.0f  // z
    );
}


bool AGridManager::GetRandomValidPlacementLocation(FVector& OutLocation)
{
    TArray<FIntPoint> FreeCells;

    for (int32 X = 0; X < GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < GridSizeY; ++Y)
        {
            FIntPoint Cell(X, Y);
            if (!OccupiedCells.Contains(Cell))
            {
                FreeCells.Add(Cell);
            }
        }
    }

    if (FreeCells.IsEmpty()) return false;

    int32 Index = FMath::RandRange(0, FreeCells.Num() - 1);
    FIntPoint Chosen = FreeCells[Index];
    OutLocation = GridToWorld(Chosen);
    return true;
}

