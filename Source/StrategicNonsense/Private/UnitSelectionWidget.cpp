#include "UnitSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

/**
 * @brief Called when the widget is constructed.
 *
 * Binds click events for Sniper and Brawler buttons,
 * and sets their text labels accordingly.
 */
void UUnitSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SniperButton)
    {
        SniperButton->OnClicked.AddDynamic(this, &UUnitSelectionWidget::OnSniperClicked);
        if (SniperLabel) SniperLabel->SetText(FText::FromString("Sniper"));
    }

    if (BrawlerButton)
    {
        BrawlerButton->OnClicked.AddDynamic(this, &UUnitSelectionWidget::OnBrawlerClicked);
        if (BrawlerLabel) BrawlerLabel->SetText(FText::FromString("Brawler"));
    }
}

/**
 * @brief Initialises the unit class options to be presented to the player.
 *
 * This must be called before the player can make a valid selection.
 *
 * @param InSniper The Sniper unit class.
 * @param InBrawler The Brawler unit class.
 */
void UUnitSelectionWidget::InitialiseOptions(TSubclassOf<AUnitActor> InSniper, TSubclassOf<AUnitActor> InBrawler)
{
    SniperClass = InSniper;
    BrawlerClass = InBrawler;
}

/**
 * @brief Called when the Sniper button is clicked.
 *
 * Broadcasts the selected Sniper class and removes the widget from the viewport.
 */
void UUnitSelectionWidget::OnSniperClicked()
{
    OnUnitChosen.Broadcast(SniperClass);
    RemoveFromParent();
}

/**
 * @brief Called when the Brawler button is clicked.
 *
 * Broadcasts the selected Brawler class and removes the widget from the viewport.
 */
void UUnitSelectionWidget::OnBrawlerClicked()
{
    OnUnitChosen.Broadcast(BrawlerClass);
    RemoveFromParent();
}
