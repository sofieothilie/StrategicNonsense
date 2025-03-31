#include "GameStatusWidget.h"
#include "UnitActor.h"


void UGameStatusWidget::SetTeamInfo(bool bIsTeam1, const FString& Colour, bool bIsAI)
{
    FString Role = bIsAI ? TEXT("AI") : TEXT("PLAYER");
    FString Text = FString::Printf(TEXT("%s: %s"), *Role, *Colour);

    if (bIsTeam1 && Team1Text)
    {
        Team1Text->SetText(FText::FromString(Text));
    }
    else if (!bIsTeam1 && Team2Text)
    {
        Team2Text->SetText(FText::FromString(Text));
    }
}


void UGameStatusWidget::SetUnitHealth(bool bIsTeam1, EGameUnitType UnitType, int32 CurrentHP, int32 MaxHP)
{
    FString TypeStr = UnitType == EGameUnitType::Sniper ? TEXT("Sniper") : TEXT("Brawler");
    FString Text = FString::Printf(TEXT("%s: %d/%d HP"), *TypeStr, CurrentHP, MaxHP);

    if (bIsTeam1)
    {
        if (UnitType == EGameUnitType::Sniper && Team1SniperText)
            Team1SniperText->SetText(FText::FromString(Text));
        else if (UnitType == EGameUnitType::Brawler && Team1BrawlerText)
            Team1BrawlerText->SetText(FText::FromString(Text));
    }
    else
    {
        if (UnitType == EGameUnitType::Sniper && Team2SniperText)
            Team2SniperText->SetText(FText::FromString(Text));
        else if (UnitType == EGameUnitType::Brawler && Team2BrawlerText)
            Team2BrawlerText->SetText(FText::FromString(Text));
    }
}
