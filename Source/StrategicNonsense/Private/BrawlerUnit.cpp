#include "BrawlerUnit.h"
#include "PaperSpriteComponent.h"

/**
 * @brief Constructor for the Brawler unit class.
 *
 * Sets default attributes for the Brawler:
 * - Movement range: 6 cells
 * - Attack type: Melee (short-range)
 * - Attack range: 1 cell
 * - Damage range: 1 to 6
 * - Health: 40 HP
 */
ABrawlerUnit::ABrawlerUnit()
{
    UnitType = EGameUnitType::Brawler;
    MaxMovement = 6;
    AttackType = EAttackType::ShortRange;
    AttackRange = 1;
    Damage = { 1, 6 };
    Health = 40;
}
