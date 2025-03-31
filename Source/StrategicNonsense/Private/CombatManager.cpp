#include "CombatManager.h"
#include "UnitActor.h"
#include "BattleGameMode.h"
#include "GridManager.h"

/**
 * @brief Initialises the combat manager with a reference to the grid and owning game mode.
 * @param Grid Pointer to the grid manager used for cell updates and position tracking.
 */
void UCombatManager::Initialise(AGridManager* Grid)
{
    GridManager = Grid;
    GameMode = Cast<ABattleGameMode>(GetOuter());
}

/**
 * @brief Executes an attack from one unit to another, applying damage and triggering counterattack logic.
 *
 * Validates that both attacker and target are alive and within range before applying damage.
 * Updates the game status widget and removes units if their health reaches zero.
 *
 * @param Attacker The unit initiating the attack.
 * @param Target The unit receiving the attack.
 * @return true if the attack was successfully executed; false otherwise.
 */
bool UCombatManager::ExecuteAttack(AUnitActor* Attacker, AUnitActor* Target)
{
    if (!Attacker || !Target || Attacker->IsDead() || Target->IsDead())
        return false;

    if (!IsInRange(Attacker, Target))
        return false;

    Attacker->ApplyDamageTo(Target);
    if (GameMode) GameMode->UpdateGameStatusWidget();
    RemoveUnitIfDead(Target);


    HandleCounterattack(Attacker, Target);
    RemoveUnitIfDead(Attacker);

    return true;
}

/**
 * @brief Checks if the target unit is within attack range of the attacker.
 * @param Attacker The attacking unit.
 * @param Target The target unit.
 * @return true if the target is within range; false otherwise.
 */
bool UCombatManager::IsInRange(AUnitActor* Attacker, AUnitActor* Target) const
{
    FIntPoint From = Attacker->GetGridPosition();
    FIntPoint To = Target->GetGridPosition();

    int32 Distance = FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);
    return Distance <= Attacker->GetAttackRange();
}

/**
 * @brief Handles counterattack logic after a successful attack.
 *
 * A Sniper receives counter damage if it attacks another Sniper,
 * or a Brawler from adjacent range (1 cell).
 *
 * @param Attacker The unit that initiated the attack (and may receive counter damage).
 * @param Target The unit that may counterattack.
 */
void UCombatManager::HandleCounterattack(AUnitActor* Attacker, AUnitActor* Target)
{
    // Only trigger counterattack under valid rules
    if (Target->IsDead()) return;

    FIntPoint A = Attacker->GetGridPosition();
    FIntPoint T = Target->GetGridPosition();
    int32 Distance = FMath::Abs(A.X - T.X) + FMath::Abs(A.Y - T.Y);

    bool IsAttackerSniper = Attacker->GetUnitType() == EGameUnitType::Sniper;

    bool ShouldCounter = IsAttackerSniper &&
        (
            Target->GetUnitType() == EGameUnitType::Sniper ||
            (Target->GetUnitType() == EGameUnitType::Brawler && Distance == 1)
            );


    if (!ShouldCounter) return;

    int32 CounterDamage = FMath::RandRange(1, 3);
    Attacker->ReceiveDamage(CounterDamage);

    UE_LOG(LogTemp, Warning, TEXT("Counterattack! %s received %d damage."),
        *UEnum::GetValueAsString(Attacker->GetUnitType()), CounterDamage);
    if (GameMode) GameMode->UpdateGameStatusWidget();

}

/**
 * @brief Removes a unit from the grid and destroys it if its health has reached zero.
 * @param Unit The unit to check and remove if dead.
 */
void UCombatManager::RemoveUnitIfDead(AUnitActor* Unit)
{
    if (Unit->IsDead())
    {
        FIntPoint Pos = Unit->GetGridPosition();
        GridManager->SetUnitAtCell(Pos, nullptr);
        Unit->Destroy(); // Or SetActorHidden + Disable
        UE_LOG(LogTemp, Warning, TEXT("%s has been eliminated."), *Unit->GetName());
        if (GameMode) GameMode->UpdateGameStatusWidget();

    }
}