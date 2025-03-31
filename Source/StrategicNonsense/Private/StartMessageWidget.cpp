#include "StartMessageWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

/**
 * @brief Sets the message text to be displayed when the widget appears.
 *
 * @param InText The message to store and later display in the UI.
 */
void UStartMessageWidget::SetMessageText(FText InText)
{
    StoredMessage = InText;
}

/**
 * @brief Called when the widget is constructed.
 *
 * Displays the stored message text and sets a timer to automatically remove the widget after 2 seconds.
 */
void UStartMessageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartTextBlock)
    {
        StartTextBlock->SetText(StoredMessage);
    }

    // Auto-remove after 2 seconds
    FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
        {
            this->RemoveFromParent();
        }, 2.0f, false);
}
