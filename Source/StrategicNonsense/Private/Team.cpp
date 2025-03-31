#include "Team.h"
#include "UnitActor.h"
#include "SniperUnit.h"
#include "BrawlerUnit.h"

/**
 * @brief Initialises the team with a colour and player/AI role.
 *
 * Loads blueprint classes for the team's Sniper and Brawler units based on the colour.
 *
 * @param Colour The team's identifying colour (used for blueprint paths).
 * @param bIsPlayer True if the team is controlled by the player, false if AI.
 */
void UTeam::Initialise(FName Colour, bool bIsPlayer)
{
    TeamColour = Colour;
    bPlayerControlled = bIsPlayer;

    FString SniperPath = FString::Printf(TEXT("/Game/Blueprints/BP_Sniper_%s.BP_Sniper_%s_C"), *TeamColour.ToString(), *TeamColour.ToString());
    FString BrawlerPath = FString::Printf(TEXT("/Game/Blueprints/BP_Brawler_%s.BP_Brawler_%s_C"), *TeamColour.ToString(), *TeamColour.ToString());

    SniperBlueprint = StaticLoadClass(AUnitActor::StaticClass(), nullptr, *SniperPath);
    BrawlerBlueprint = StaticLoadClass(AUnitActor::StaticClass(), nullptr, *BrawlerPath);

    if (!SniperBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Sniper blueprint for team: %s"), *TeamColour.ToString());
    }

    if (!BrawlerBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Brawler blueprint for team: %s"), *TeamColour.ToString());
    }

    UnitsLeftToPlace = { SniperBlueprint, BrawlerBlueprint };

}

/**
 * @brief Gets the team's assigned colour.
 * @return The colour name.
 */
FName UTeam::GetTeamColour() const
{
    return TeamColour;
}

/**
 * @brief Returns the blueprint class for the Sniper unit.
 * @return The Sniper unit class.
 */
TSubclassOf<AUnitActor> UTeam::GetSniperBlueprint() const
{
    return SniperBlueprint;
}

/**
 * @brief Returns the blueprint class for the Brawler unit.
 * @return The Brawler unit class.
 */
TSubclassOf<AUnitActor> UTeam::GetBrawlerBlueprint() const
{
    return BrawlerBlueprint;
}

/**
 * @brief Indicates whether the team is player-controlled.
 * @return True if player-controlled; false if AI.
 */
bool UTeam::IsPlayerControlled() const
{
    return bPlayerControlled;
}

/**
 * @brief Returns a reference to the list of units not yet placed on the grid.
 * @return Array of unit classes to place.
 */
TArray<TSubclassOf<AUnitActor>>& UTeam::GetUnplacedUnits()
{
    return UnitsLeftToPlace;
}

/**
 * @brief Checks if all living units on this team have completed their turn.
 * @return True if no unit has remaining actions; false otherwise.
 */
bool UTeam::HasTeamFinishedTurn() const
{
    for (AUnitActor* Unit : ControlledUnits)
    {
        if (Unit && !Unit->IsDead() && !Unit->HasMovedThisTurn())
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Adds a unit to the team’s list of controlled units.
 * @param Unit Pointer to the unit to add.
 */
void UTeam::AddUnit(AUnitActor* Unit)
{
    if (Unit)
    {
        ControlledUnits.Add(Unit);
    }
}

/**
 * @brief Gets all units currently controlled by this team.
 * @return Const reference to the array of controlled units.
 */
const TArray<AUnitActor*>& UTeam::GetControlledUnits() const
{
    return ControlledUnits;
}

/**
 * @brief Resets all controlled units for a new turn (movement and attack).
 */
void UTeam::ResetUnitsForNewTurn()
{
    for (AUnitActor* Unit : ControlledUnits)
    {
        if (Unit)
        {
            Unit->ResetMovement();
            Unit->ResetAttack();
        }
    }
}

/**
 * @brief Checks whether a given unit belongs to this team.
 * @param Unit The unit to check.
 * @return True if the unit is controlled by this team.
 */
bool UTeam::OwnsUnit(const AUnitActor* Unit) const
{
    return ControlledUnits.Contains(Unit);
}

/**
 * @brief Checks whether the team has any living units.
 * @return True if at least one unit is alive; false otherwise.
 */
bool UTeam::HasLivingUnits() const
{
    for (AUnitActor* Unit : ControlledUnits)
    {
        if (IsValid(Unit) && !Unit->IsDead())
            return true;
    }
    return false;
}

