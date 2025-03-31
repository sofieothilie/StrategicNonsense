#include "Obstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

/**
 * @brief Constructor for the obstacle actor.
 *
 * Sets up the static mesh component and disables ticking.
 */
AObstacle::AObstacle()
{
    PrimaryActorTick.bCanEverTick = false;

    ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
    RootComponent = ObstacleMesh;
}

/**
 * @brief Called when the game starts.
 *
 * Applies a random texture and sets the obstacle's scale based on the override.
 */
void AObstacle::BeginPlay()
{
    Super::BeginPlay();
    ApplyRandomTexture();
    SetActorScale3D(ScaleOverride);
}

/**
 * @brief Randomly selects and applies a texture from the ObstacleTextures array.
 *
 * Uses a dynamic material instance to assign the selected texture to the mesh.
 * Logs warnings if the texture array is empty or contains null entries.
 */
void AObstacle::ApplyRandomTexture()
{
    if (ObstacleTextures.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No textures assigned to ObstacleTextures array!"));
        return;
    }

    // Pick a random texture from the array
    int32 RandomIndex = FMath::RandRange(0, ObstacleTextures.Num() - 1);
    UTexture2D* SelectedTexture = ObstacleTextures[RandomIndex];

    if (!SelectedTexture)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected texture is null!"));
        return;
    }

    // Create a dynamic material instance
    UMaterialInstanceDynamic* DynamicMaterial = ObstacleMesh->CreateDynamicMaterialInstance(0);
    if (DynamicMaterial)
    {
        DynamicMaterial->SetTextureParameterValue(TEXT("BaseTexture"), SelectedTexture);
    }
}
