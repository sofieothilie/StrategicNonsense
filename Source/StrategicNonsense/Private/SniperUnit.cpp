#include "SniperUnit.h"
#include "PaperSpriteComponent.h"

/**
 * @brief Constructor for the Sniper unit class.
 *
 * Sets default attributes for the Sniper:
 * - Movement range: 3 cells
 * - Attack type: Ranged (ignores obstacles)
 * - Attack range: 10 cells
 * - Damage range: 4 to 8
 * - Health: 20 HP
 */
ASniperUnit::ASniperUnit()
{
    UnitType = EGameUnitType::Sniper;
    MaxMovement = 3;
    AttackType = EAttackType::Ranged;
    AttackRange = 10;
    Damage = { 4, 8 };
    Health = 20;
}
