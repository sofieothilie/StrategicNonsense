#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActor.generated.h"

class UPaperSpriteComponent;

UENUM(BlueprintType)
enum class EGameUnitType : uint8
{
    Sniper,
    Brawler
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    ShortRange,
    Ranged
};

USTRUCT(BlueprintType)
struct FDamageRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Min;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Max;
};

UCLASS(Abstract)
class STRATEGICNONSENSE_API AUnitActor : public AActor
{
    GENERATED_BODY()

public:
    AUnitActor();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    int32 GetRandomDamage() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyDamageTo(AUnitActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ReceiveDamage(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsDead() const;
    FIntPoint GetGridPosition() const { return GridPosition; }
    void SetGridPosition(FIntPoint NewPos) { GridPosition = NewPos; }

    bool HasMovedThisTurn() const { return bHasMoved; }
    void MarkAsMoved() { bHasMoved = true; }
    void ResetMovement() { bHasMoved = false; }

    virtual int32 GetMovementRange() const { return 0; } // Default: immobile

    int32 GetAttackRange() const { return AttackRange; }
    EGameUnitType GetUnitType() const { return UnitType; }



protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
    UPaperSpriteComponent* SpriteComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EGameUnitType UnitType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    EAttackType AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FDamageRange Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Health;

    FIntPoint GridPosition;
    bool bHasMoved = false;
};
