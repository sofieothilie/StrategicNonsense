#include "GameOverWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
    }
}

void UGameOverWidget::OnRestartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Restart button clicked. Restarting level..."));
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


void UGameOverWidget::SetResultText(const FString& Result)
{
    if (UTextBlock* Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultText"))))
    {
        Text->SetText(FText::FromString(Result));
    }
}


