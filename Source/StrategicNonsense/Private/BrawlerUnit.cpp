#include "BrawlerUnit.h"
#include "PaperSpriteComponent.h"

ABrawlerUnit::ABrawlerUnit()
{
    UnitType = EGameUnitType::Brawler;
    MaxMovement = 6;
    AttackType = EAttackType::ShortRange;
    AttackRange = 1;
    Damage = { 1, 6 };
    Health = 40;
}
