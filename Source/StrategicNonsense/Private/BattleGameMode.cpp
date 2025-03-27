#include "BattleGameMode.h"
#include "GridManager.h"
#include "GridCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Team.h"
#include "SniperUnit.h"
#include "UnitPlacementManager.h"
#include "BattlePlayerController.h"
#include "Misc/OutputDeviceNull.h"
#include "Blueprint/UserWidget.h"
#include "StartMessageWidget.h"
#include "UnitSelectionWidget.h"


ABattleGameMode::ABattleGameMode()
{
    PlayerControllerClass = ABattlePlayerController::StaticClass();
}

void ABattleGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTopDownCamera();
    SpawnGridAndSetup();

    DecideStartingPlayer();
    SetupTeams();
    UnitPlacementManager = NewObject<UUnitPlacementManager>(this);
    UnitPlacementManager->Initialise(this, SpawnedGridManager, AllTeams, bPlayerStarts);
}

void ABattleGameMode::SpawnTopDownCamera()
{
    FVector Location(1000.f, 1000.f, 3000.f);
    FRotator Rotation(-90.f, 0.f, 0.f);
    GetWorld()->SpawnActor<AGridCameraActor>(Location, Rotation);
}

void ABattleGameMode::SpawnGridAndSetup()
{
    FVector Location(0.f, 0.f, 0.f);
    FRotator Rotation(0.f, 0.f, 0.f);

    SpawnedGridManager = GetWorld()->SpawnActor<AGridManager>(Location, Rotation);
    if (!SpawnedGridManager) return;

    SpawnedGridManager->GenerateGrid();
    SpawnedGridManager->PlaceObstacles();
}

void ABattleGameMode::DecideStartingPlayer()
{
    bPlayerStarts = FMath::RandBool();
}

void ABattleGameMode::SetupTeams()
{
    TArray<FName> AvailableColours = { "Blue", "Red", "Violet", "Brown", "Green" };

    for (int32 i = AvailableColours.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        AvailableColours.Swap(i, j);
    }

    FName Colour1 = AvailableColours[0];
    FName Colour2 = AvailableColours[1];

    Team1 = NewObject<UTeam>(this);
    Team1->Initialise(Colour1, true);

    Team2 = NewObject<UTeam>(this);
    Team2->Initialise(Colour2, false);

    AllTeams.Add(Team1);
    AllTeams.Add(Team2);

    UE_LOG(LogTemp, Warning, TEXT("Selected Teams: %s for player and %s for AI"), *Colour1.ToString(), *Colour2.ToString());
}

void ABattleGameMode::OnPlayerClickedGrid(const FVector& ClickLocation)
{

    if (CurrentPhase == EGamePhase::Placement && UnitPlacementManager)
    {
        UnitPlacementManager->HandlePlayerClickedGrid(ClickLocation);
    }
}

void ABattleGameMode::SetGamePhase(EGamePhase NewPhase)
{
    CurrentPhase = NewPhase;

    switch (NewPhase)
    {
    case EGamePhase::PlayerTurn:
    {
        UTeam* PlayerTeam = GetPlayerTeam();
        if (PlayerTeam)
        {
            PlayerTeam->ResetUnitsMovement();
            UE_LOG(LogTemp, Warning, TEXT("Player units reset for new turn."));
        }
        break;
    }
    case EGamePhase::AITurn:
    {
        UTeam* AITeam = GetPlayerTeam() == Team1 ? Team2 : Team1;
        if (AITeam)
        {
            AITeam->ResetUnitsMovement();
            UE_LOG(LogTemp, Warning, TEXT("AI units reset for new turn."));
        }
        break;
    }
    default:
        break;
    }
}



UTeam* ABattleGameMode::GetPlayerTeam() const
{ 
    return Team1->IsPlayerControlled() ? Team1 : Team2;
}
