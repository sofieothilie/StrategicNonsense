#include "EndTurnWidget.h"
#include "Components/Button.h"
#include "BattlePlayerController.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Called when the widget is constructed.
 *
 * Binds the OnClicked event of the End Turn button to the handler function.
 * Ensures that the button interaction is functional at runtime.
 */
void UEndTurnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (EndTurnButton)
    {
        EndTurnButton->OnClicked.AddDynamic(this, &UEndTurnWidget::OnEndTurnClicked);
    }
}


/**
 * @brief Handles the End Turn button click.
 *
 * Retrieves the player controller and delegates the end-turn logic to it.
 * Transitions the game from the player's turn to the AI's turn.
 */
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
