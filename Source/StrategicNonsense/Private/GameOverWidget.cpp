#include "GameOverWidget.h"
#include "Components/TextBlock.h"

void UGameOverWidget::SetResultText(const FString& Result)
{
    if (UTextBlock* Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultText"))))
    {
        Text->SetText(FText::FromString(Result));
    }
}
