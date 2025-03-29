#include "Team.h"
#include "UnitActor.h"
#include "SniperUnit.h"
#include "BrawlerUnit.h"

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

FName UTeam::GetTeamColour() const
{
    return TeamColour;
}

TSubclassOf<AUnitActor> UTeam::GetSniperBlueprint() const
{
    return SniperBlueprint;
}

TSubclassOf<AUnitActor> UTeam::GetBrawlerBlueprint() const
{
    return BrawlerBlueprint;
}

bool UTeam::IsPlayerControlled() const
{
    return bPlayerControlled;
}

TArray<TSubclassOf<AUnitActor>>& UTeam::GetUnplacedUnits()
{
    return UnitsLeftToPlace;
}


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


void UTeam::AddUnit(AUnitActor* Unit)
{
    if (Unit)
    {
        ControlledUnits.Add(Unit);
    }
}

const TArray<AUnitActor*>& UTeam::GetControlledUnits() const
{
    return ControlledUnits;
}


void UTeam::ResetUnitsMovement()
{
    for (AUnitActor* Unit : ControlledUnits)
    {
        if (Unit)
        {
            Unit->ResetMovement();
        }
    }
}


bool UTeam::OwnsUnit(const AUnitActor* Unit) const
{
    return ControlledUnits.Contains(Unit);
}


bool UTeam::HasLivingUnits() const
{
    for (AUnitActor* Unit : ControlledUnits)
    {
        if (IsValid(Unit) && !Unit->IsDead())
            return true;
    }
    return false;
}

