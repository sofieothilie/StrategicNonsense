#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameOverWidget.generated.h"

/**
 * @class UGameOverWidget
 * @brief Final game state display when one player wins.
 *
 * Displays the match result (Victory/Defeat).
 * Allows players to restart the match.
 */



UCLASS()
class STRATEGICNONSENSE_API UGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetResultText(const FString& Result);

    UPROPERTY(meta = (BindWidget))
    class UButton* RestartButton;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnRestartClicked();

};
