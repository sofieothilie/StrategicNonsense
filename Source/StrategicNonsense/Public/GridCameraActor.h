#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCameraActor.generated.h"

/**
 * @class AGridCameraActor
 * @brief Provides a fixed 2D top-down view of the entire game grid.
 *
 * Ensures the grid remains fully visible regardless of screen resolution.
 * Useful for keeping gameplay centred and consistent in Paper2D.
 */


UCLASS()
class STRATEGICNONSENSE_API AGridCameraActor : public AActor
{
    GENERATED_BODY()

public:
    AGridCameraActor();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* CameraComponent;

    void ConfigureCamera();
};
