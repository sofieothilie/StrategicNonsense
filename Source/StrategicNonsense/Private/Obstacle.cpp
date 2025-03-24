#include "Obstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AObstacle::AObstacle()
{
    PrimaryActorTick.bCanEverTick = false;

    ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
    RootComponent = ObstacleMesh;
}

void AObstacle::BeginPlay()
{
    Super::BeginPlay();
    ApplyRandomTexture();
    SetActorScale3D(ScaleOverride);
}


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
