// GridCameraActor.cpp
#include "GridCameraActor.h"
#include "GridManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Constructor for the grid camera actor.
 *
 * Sets up an orthographic camera as the root component for a top-down view.
 * Disables ticking since the camera remains static.
 */
AGridCameraActor::AGridCameraActor()
{
    PrimaryActorTick.bCanEverTick = false;
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    RootComponent = CameraComponent;

    CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
}

/**
 * @brief Called when the game starts or the actor is spawned.
 *
 * Forces the player controller to use this camera as the view target,
 * and configures the camera to frame the grid properly.
 */
void AGridCameraActor::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Forcing Camera View Target!"));
        PlayerController->SetViewTarget(this);

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Player Controller Found!"));
    }

    ConfigureCamera();


}

/**
 * @brief Positions and configures the orthographic camera to centre and frame the game grid.
 *
 * Sets location, rotation, and width based on a 25x25 grid.
 */
void AGridCameraActor::ConfigureCamera()
{
    const float CellSize = 100.f; // Adjust based on your actual cell size
    const int32 GridSize = 25;

    const float GridWorldSize = GridSize * CellSize;

    // Centre the camera
    SetActorLocation(FVector(GridWorldSize / 2, GridWorldSize / 2, 1000.f)); // Z value high enough for top-down

    // Make it look straight down
    SetActorRotation(FRotator(-90.f, 0.f, 0.f));

    // Set the orthographic width to fit the grid
    CameraComponent->OrthoWidth = GridWorldSize * 1.5f; // Slight padding
}
