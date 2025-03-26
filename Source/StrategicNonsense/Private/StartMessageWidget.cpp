#include "StartMessageWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UStartMessageWidget::SetMessageText(FText InText)
{
    StoredMessage = InText;
}

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
