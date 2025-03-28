// CombatSystem.cpp
#include "CombatManager.h"
#include "UnitActor.h"
#include "GridManager.h"

void UCombatManager::Initialise(AGridManager* Grid)
{
    GridManager = Grid;
}

bool UCombatManager::ExecuteAttack(AUnitActor* Attacker, AUnitActor* Target)
{
    if (!Attacker || !Target || Attacker->IsDead() || Target->IsDead())
        return false;

    if (!IsInRange(Attacker, Target))
        return false;

    Attacker->ApplyDamageTo(Target);
    RemoveUnitIfDead(Target);

    HandleCounterattack(Attacker, Target);
    RemoveUnitIfDead(Attacker);

    return true;
}

bool UCombatManager::IsInRange(AUnitActor* Attacker, AUnitActor* Target) const
{
    FIntPoint From = Attacker->GetGridPosition();
    FIntPoint To = Target->GetGridPosition();

    int32 Distance = FMath::Abs(From.X - To.X) + FMath::Abs(From.Y - To.Y);
    return Distance <= Attacker->GetAttackRange();
}

void UCombatManager::HandleCounterattack(AUnitActor* Attacker, AUnitActor* Target)
{
    // Only trigger counterattack under valid rules
    if (Target->IsDead()) return;

    FIntPoint A = Attacker->GetGridPosition();
    FIntPoint T = Target->GetGridPosition();
    int32 Distance = FMath::Abs(A.X - T.X) + FMath::Abs(A.Y - T.Y);

    bool ShouldCounter =
        Target->GetUnitType() == EGameUnitType::Sniper ||
        (Target->GetUnitType() == EGameUnitType::Brawler && Distance == 1);

    if (!ShouldCounter) return;

    int32 CounterDamage = FMath::RandRange(1, 3);
    Attacker->ReceiveDamage(CounterDamage);

    UE_LOG(LogTemp, Warning, TEXT("Counterattack! %s received %d damage."),
        *UEnum::GetValueAsString(Attacker->GetUnitType()), CounterDamage);
}

void UCombatManager::RemoveUnitIfDead(AUnitActor* Unit)
{
    if (Unit->IsDead())
    {
        FIntPoint Pos = Unit->GetGridPosition();
        GridManager->SetUnitAtCell(Pos, nullptr);
        Unit->Destroy(); // Or SetActorHidden + Disable
        UE_LOG(LogTemp, Warning, TEXT("%s has been eliminated."), *Unit->GetName());
    }
}
