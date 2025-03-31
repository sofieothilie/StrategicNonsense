#include "SniperUnit.h"
#include "PaperSpriteComponent.h"

ASniperUnit::ASniperUnit()
{
    UnitType = EGameUnitType::Sniper;
    MaxMovement = 3;
    AttackType = EAttackType::Ranged;
    AttackRange = 10;
    Damage = { 4, 8 };
    Health = 20;
}
