#include "UnitActor.h"
#include "PaperSpriteComponent.h"

/**
 * @brief Base constructor for all unit types (Sniper, Brawler).
 *
 * Sets default values for movement, attack range, damage, and health.
 * Sprite component is expected to be assigned via Blueprint.
 */
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

/**
 * @brief Calculates and returns a random damage value within the unit's damage range.
 * @return A randomly selected integer between Damage.Min and Damage.Max.
 */
int32 AUnitActor::GetRandomDamage() const
{
    return FMath::RandRange(Damage.Min, Damage.Max);
}

/**
 * @brief Applies damage to a target unit.
 *
 * Computes random damage and reduces the target's health.
 * Skips damage if the target is already dead.
 *
 * @param Target The unit to attack.
 */
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

/**
 * @brief Reduces this unit’s health by a specified amount.
 *
 * Prevents health from dropping below zero.
 *
 * @param Amount The amount of damage to apply.
 */
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

/**
 * @brief Checks if the unit is dead (i.e., health is zero or below).
 * @return True if the unit is dead.
 */
bool AUnitActor::IsDead() const
{
    return Health <= 0;
}
