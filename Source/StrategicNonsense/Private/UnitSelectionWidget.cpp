#include "UnitSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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

void UUnitSelectionWidget::InitialiseOptions(TSubclassOf<AUnitActor> InSniper, TSubclassOf<AUnitActor> InBrawler)
{
    SniperClass = InSniper;
    BrawlerClass = InBrawler;
}

void UUnitSelectionWidget::OnSniperClicked()
{
    OnUnitChosen.Broadcast(SniperClass);
    RemoveFromParent();
}

void UUnitSelectionWidget::OnBrawlerClicked()
{
    OnUnitChosen.Broadcast(BrawlerClass);
    RemoveFromParent();
}
