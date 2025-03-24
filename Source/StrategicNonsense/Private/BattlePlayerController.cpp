#include "BattlePlayerController.h"
#include "BattleGameMode.h"
#include "Kismet/GameplayStatics.h"

void ABattlePlayerController::SetupInputComponent()
{
    UE_LOG(LogTemp, Warning, TEXT("Inside playercontroller"));

    Super::SetupInputComponent();
    InputComponent->BindAction("LeftClick", IE_Pressed, this, &ABattlePlayerController::HandleLeftClick);
}

void ABattlePlayerController::HandleLeftClick()
{
    UE_LOG(LogTemp, Warning, TEXT("Inside playercontroller, left click"));

    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (Hit.bBlockingHit)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(this));
        if (GameMode)
        {
            GameMode->OnPlayerClickedGrid(Hit.Location);
        }
    }
}
