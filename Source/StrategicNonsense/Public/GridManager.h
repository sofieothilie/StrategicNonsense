#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

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

    void SetBlueprints(TSubclassOf<AActor> InCell, TSubclassOf<AActor> InTree1, TSubclassOf<AActor> InTree2, TSubclassOf<AActor> InMountain);

    UPROPERTY() TSubclassOf<AActor> CellBlueprint;
    UPROPERTY() TSubclassOf<AActor> BP_Tree1;
    UPROPERTY() TSubclassOf<AActor> BP_Tree2;
    UPROPERTY() TSubclassOf<AActor> BP_Mountain;

    bool TryPlaceUnitAtLocation(const FVector& ClickLocation, TSubclassOf<class AUnitActor> UnitToPlace);

    UFUNCTION(BlueprintCallable)
    bool GetRandomValidPlacementLocation(FVector& OutLocation);


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
    FIntPoint WorldToGrid(const FVector& Location) const;
    FVector GridToWorld(const FIntPoint& Cell) const;
    TSet<FIntPoint> OccupiedCells;

};
