#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCameraActor.generated.h"

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
