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
    ObstacleSizes.Add(BP_Mountain, FIntPoint(8, 8));
    ObstacleSizes.Add(BP_Tree1, FIntPoint(1, 1));
    ObstacleSizes.Add(BP_Tree2, FIntPoint(1, 1));

    TArray<FIntPoint> MountainShape = {
    {1, 0}, {2, 0}, {3, 1},
    {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1},
    {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2},
    {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3},
    {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4},
    {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5},
    {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6},
    {3, 7}, {4, 7}, {5, 7}
    };


    float GroundHeightOffset = 50.0f;

    while (OccupiedCells.Num() < NumObstacles)
    {
        TSubclassOf<AActor> ChosenObstacle = ObstacleTypes[FMath::RandRange(0, ObstacleTypes.Num() - 1)];
        FIntPoint ObstacleSize = ObstacleSizes[ChosenObstacle];

        int32 OriginX = FMath::RandRange(0, GridSizeX - ObstacleSize.X);
        int32 OriginY = FMath::RandRange(0, GridSizeY - ObstacleSize.Y);
        FIntPoint OriginCell = FIntPoint(OriginX, OriginY);

        bool CanPlace = true;
        TArray<FIntPoint> CellsToOccupy;

        if (ChosenObstacle == BP_Mountain)
        {
            for (const FIntPoint& Offset : MountainShape)
            {
                FIntPoint TestCell = OriginCell + Offset;
                if (!IsCellValid(TestCell) || OccupiedCells.Contains(TestCell))
                {
                    CanPlace = false;
                    break;
                }
                CellsToOccupy.Add(TestCell);
            }
        }
        else
        {
            for (int32 i = 0; i < ObstacleSize.X; ++i)
            {
                for (int32 j = 0; j < ObstacleSize.Y; ++j)
                {
                    FIntPoint TestCell(OriginX + i, OriginY + j);
                    if (OccupiedCells.Contains(TestCell))
                    {
                        CanPlace = false;
                        break;
                    }
                    CellsToOccupy.Add(TestCell);
                }
                if (!CanPlace) break;
            }
        }

        if (!CanPlace) continue;

        FVector SpawnLocation = FVector(
            (OriginX + ObstacleSize.X / 2.0f) * CellSize,
            (OriginY + ObstacleSize.Y / 2.0f) * CellSize,
            GroundHeightOffset
        );

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
                OriginX, OriginY, SpawnLocation.X, SpawnLocation.Y);
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
    NewUnit->SetGridPosition(GridCoord);

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


bool AGridManager::IsCellWalkable(const FIntPoint& Cell) const
{
    if (!IsCellValid(Cell))
        return false;

    return !OccupiedCells.Contains(Cell);
}

void AGridManager::SetUnitAtCell(const FIntPoint& Cell, AUnitActor* Unit)
{
    if (!IsCellValid(Cell))
        return;

    if (Unit)
    {
        OccupiedCells.Add(Cell);
        UE_LOG(LogTemp, Warning, TEXT("Set unit at cell (%d, %d)"), Cell.X, Cell.Y);
    }
    else
    {
        OccupiedCells.Remove(Cell);
        UE_LOG(LogTemp, Warning, TEXT("Cleared unit at cell (%d, %d)"), Cell.X, Cell.Y);
    }
}

TSet<FIntPoint> AGridManager::FindReachableCellsBFS(FIntPoint StartCell, int32 MaxRange) const
{
    TSet<FIntPoint> Visited;
    TQueue<TPair<FIntPoint, int32>> Frontier;

    Visited.Add(StartCell);
    Frontier.Enqueue(TPair<FIntPoint, int32>(StartCell, 0));

    const TArray<FIntPoint> Directions = {
        FIntPoint(1, 0),
        FIntPoint(-1, 0),
        FIntPoint(0, 1),
        FIntPoint(0, -1)
    };

    while (!Frontier.IsEmpty())
    {
        TPair<FIntPoint, int32> Current;
        Frontier.Dequeue(Current);

        FIntPoint Cell = Current.Key;
        int32 Distance = Current.Value;

        if (Distance >= MaxRange)
            continue;

        for (const FIntPoint& Dir : Directions)
        {
            FIntPoint Neighbor = Cell + Dir;

            if (!IsCellValid(Neighbor)) continue;
            if (OccupiedCells.Contains(Neighbor)) continue;
            if (Visited.Contains(Neighbor)) continue;

            Visited.Add(Neighbor);
            Frontier.Enqueue(TPair<FIntPoint, int32>(Neighbor, Distance + 1));
        }
    }

    Visited.Remove(StartCell); // Optional: don't count standing cell
    return Visited;
}

AUnitActor* AGridManager::SpawnAndPlaceUnit(const FIntPoint& GridCoord, TSubclassOf<AUnitActor> UnitClass)
{
    if (!IsCellValid(GridCoord) || OccupiedCells.Contains(GridCoord))
        return nullptr;

    FVector SpawnLocation = GridToWorld(GridCoord);
    FRotator Rotation = FRotator(0.f, 0.f, 90.f);

    AUnitActor* NewUnit = GetWorld()->SpawnActor<AUnitActor>(UnitClass, SpawnLocation, Rotation);
    if (!NewUnit)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn unit at (%d, %d)"), GridCoord.X, GridCoord.Y);
        return nullptr;
    }

    OccupiedCells.Add(GridCoord);
    NewUnit->SetGridPosition(GridCoord);

    float PaddingFactor = NewUnit->IsA(ASniperUnit::StaticClass()) ? 0.1f : 0.15f;
    float UnitScale = (CellSize / 100.f) * PaddingFactor;
    NewUnit->SetActorScale3D(FVector(UnitScale));

    NewUnit->SetFolderPath(FName("Units"));

    UE_LOG(LogTemp, Warning, TEXT("Spawned and placed %s at (%d, %d)"), *NewUnit->GetName(), GridCoord.X, GridCoord.Y);
    return NewUnit;
}
