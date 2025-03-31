#include "GameStatusWidget.h"
#include "UnitActor.h"

/**
 * @brief Sets the displayed information for a team.
 *
 * Displays the team’s role (PLAYER or AI) and its assigned colour.
 *
 * @param bIsTeam1 Whether the team is Team 1 or not (Team 2).
 * @param Colour The colour name assigned to the team.
 * @param bIsAI True if the team is AI-controlled; false if it's the player.
 */
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

/**
 * @brief Updates the displayed health text for a specific unit.
 *
 * Shows the current and max HP of a Sniper or Brawler on either team.
 *
 * @param bIsTeam1 Whether the unit belongs to Team 1 or not (Team 2).
 * @param UnitType The type of unit (Sniper or Brawler).
 * @param CurrentHP The current health of the unit.
 * @param MaxHP The maximum health of the unit.
 */
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

/**
 * @brief Sets the turn indicator text in the UI.
 *
 * Typically displays "Player Turn" or "AI Turn".
 *
 * @param TurnDescription The description text to display.
 */
void UGameStatusWidget::SetTurnText(const FString& TurnDescription)
{
    if (Turntext)
    {
        Turntext->SetText(FText::FromString(TurnDescription));
    }
}
