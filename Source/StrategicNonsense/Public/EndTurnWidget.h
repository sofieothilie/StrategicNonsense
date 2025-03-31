#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndTurnWidget.generated.h"

/**
 * @class UEndTurnWidget
 * @brief Widget allowing the player to manually end their turn.
 *
 * Is shown during players turn, and disables further input.
 * Allows the player to only either attack or move during the game.
 */



class UButton;

UCLASS()
class STRATEGICNONSENSE_API UEndTurnWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* EndTurnButton;

    UFUNCTION()
    void OnEndTurnClicked();
};
