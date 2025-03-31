#include "EndTurnWidget.h"
#include "Components/Button.h"
#include "BattlePlayerController.h"
#include "Kismet/GameplayStatics.h"

void UEndTurnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (EndTurnButton)
    {
        EndTurnButton->OnClicked.AddDynamic(this, &UEndTurnWidget::OnEndTurnClicked);
    }
}

void UEndTurnWidget::OnEndTurnClicked()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ABattlePlayerController* BattlePC = Cast<ABattlePlayerController>(PC))
        {
            BattlePC->HandleEndTurnPressed();
        }
    }
}
