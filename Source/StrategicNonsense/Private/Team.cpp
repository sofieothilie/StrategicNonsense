#include "Team.h"
#include "UnitActor.h"
#include "SniperUnit.h"
#include "BrawlerUnit.h"

void UTeam::Initialise(FName InTeamColour)
{
    TeamColour = InTeamColour;

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
