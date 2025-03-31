#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

/**
 * @class AGridManager
 * @brief Manages the game grid and unit occupancy.
 *
 * Initializes the 25x25 grid with labelled cells (A1–Y25).
 * Tracks occupied cells, retrieves random free positions, and supports obstacle generation.
 */


UCLASS()
class STRATEGICNONSENSE_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

    UFUNCTION(BlueprintCallable)
    void GenerateGrid();

    UFUNCTION(BlueprintCallable)
    void PlaceObstacles();

    void SetBlueprints();

    bool TryPlaceUnitAtLocation(const FVector& ClickLocation, TSubclassOf<class AUnitActor> UnitToPlace);

    UFUNCTION(BlueprintCallable)
    bool GetRandomValidPlacementLocation(FVector& OutLocation);
    FIntPoint WorldToGrid(const FVector& Location) const;
    FVector GridToWorld(const FIntPoint& Cell) const;


    bool IsCellWalkable(const FIntPoint& Cell) const;
    void SetUnitAtCell(const FIntPoint& Cell, class AUnitActor* Unit);

    TSet<FIntPoint> FindReachableCellsBFS(FIntPoint StartCell, int32 MaxRange) const;

    AUnitActor* SpawnAndPlaceUnit(const FIntPoint& GridCoord, TSubclassOf<AUnitActor> UnitClass);


protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridSizeX = 25;

    UPROPERTY(EditAnywhere, Category = "Grid")
    int32 GridSizeY = 25;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float CellSize = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Obstacles")
    float ObstaclePercentage = 10.0f;

    bool IsCellValid(const FIntPoint& Cell) const;
    TSet<FIntPoint> OccupiedCells;

    UPROPERTY()
    TSubclassOf<AGridManager> GridManagerClass;

    UPROPERTY()
    TSubclassOf<AActor> CellBlueprint;

    UPROPERTY()
    TSubclassOf<AActor> BP_Tree1;

    UPROPERTY()
    TSubclassOf<AActor> BP_Tree2;

    UPROPERTY()
    TSubclassOf<AActor> BP_Mountain;

    bool WouldBlockConnectivity(const TArray<FIntPoint>& ProposedObstacle);

};
