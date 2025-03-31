#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

/**
 * @class AObstacle
 * @brief Represents a static obstacle on the game grid.
 *
 * Obstacles occupy a cell and block unit movement.
 * Types (e.g., tree, mountain) are visually distinct but functionally identical.
 */


UCLASS()
class STRATEGICNONSENSE_API AObstacle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to apply random texture
	void ApplyRandomTexture();


private:
	// Mesh component for the obstacle
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ObstacleMesh;

	// Array of textures to randomly apply to the obstacle
	UPROPERTY(EditAnywhere, Category = "Obstacle")
	TArray<UTexture2D*> ObstacleTextures;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector ScaleOverride = FVector(1.0f, 1.0f, 1.0f);

};
