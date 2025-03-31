#include "GameOverWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Called when the widget is constructed.
 *
 * Binds the Restart button's click event to the restart handler.
 * Ensures that restart functionality is active when the widget is shown.
 */
void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
    }
}

/**
 * @brief Handles the click on the Restart button.
 *
 * Reloads the current level to restart the game from the beginning.
 */
void UGameOverWidget::OnRestartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Restart button clicked. Restarting level..."));
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

/**
 * @brief Sets the result text shown in the game over screen.
 *
 * @param Result The string to display (e.g., "Red Wins!", "Draw!").
 */
void UGameOverWidget::SetResultText(const FString& Result)
{
    if (UTextBlock* Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultText"))))
    {
        Text->SetText(FText::FromString(Result));
    }
}


