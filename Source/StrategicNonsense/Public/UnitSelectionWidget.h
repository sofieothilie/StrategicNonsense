#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitSelectionWidget.generated.h"

/**
 * @class UUnitSelectionWidget
 * @brief Lets the player choose between Sniper and Brawler during placement.
 *
 * Contains two buttons mapped to each unit class.
 * Uses dynamic multicast delegates to notify listeners of the chosen unit.
 * Displayed only during the placement phase.
 */


class AUnitActor;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitChosen, TSubclassOf<AUnitActor>, ChosenUnit);

UCLASS()
class STRATEGICNONSENSE_API UUnitSelectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void InitialiseOptions(TSubclassOf<AUnitActor> InSniper, TSubclassOf<AUnitActor> InBrawler);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnUnitChosen OnUnitChosen;

protected:
    UFUNCTION()
    void OnSniperClicked();

    UFUNCTION()
    void OnBrawlerClicked();

    UPROPERTY(meta = (BindWidget))
    UButton* SniperButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BrawlerButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SniperLabel;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BrawlerLabel;

private:
    TSubclassOf<AUnitActor> SniperClass;
    TSubclassOf<AUnitActor> BrawlerClass;
};
