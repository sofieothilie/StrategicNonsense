// GridCameraActor.cpp
#include "GridCameraActor.h"
#include "GridManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AGridCameraActor::AGridCameraActor()
{
    PrimaryActorTick.bCanEverTick = false;
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    RootComponent = CameraComponent;

    CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
}

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
