#include "UnitActor.h"
#include "PaperSpriteComponent.h"

AUnitActor::AUnitActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Let Blueprint define and attach SpriteComponent (don’t override here)
    UnitType = EGameUnitType::Sniper;
    MaxMovement = 0;
    AttackType = EAttackType::ShortRange;
    AttackRange = 0;
    Damage = { 0, 0 };
    Health = 0;
}

int32 AUnitActor::GetRandomDamage() const
{
    return FMath::RandRange(Damage.Min, Damage.Max);
}

void AUnitActor::ApplyDamageTo(AUnitActor* Target)
{
    if (!Target || Target->IsDead())
        return;

    int32 DamageDealt = GetRandomDamage();
    Target->ReceiveDamage(DamageDealt);

    UE_LOG(LogTemp, Log, TEXT("%s dealt %d damage to %s"),
        *UEnum::GetValueAsString(UnitType),
        DamageDealt,
        *UEnum::GetValueAsString(Target->UnitType));
}

void AUnitActor::ReceiveDamage(int32 Amount)
{
    Health -= Amount;

    if (Health < 0)
        Health = 0;

    UE_LOG(LogTemp, Log, TEXT("%s took %d damage. Remaining health: %d"),
        *UEnum::GetValueAsString(UnitType),
        Amount,
        Health);
}

bool AUnitActor::IsDead() const
{
    return Health <= 0;
}
