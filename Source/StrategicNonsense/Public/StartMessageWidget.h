#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMessageWidget.generated.h"

/**
 * @class UStartMessageWidget
 * @brief Displays the start-of-game message (e.g. coin toss result).
 *
 * Created and shown dynamically via C++ at match start.
 * Communicates who won the coin toss and begins unit placement.
 */


UCLASS()
class STRATEGICNONSENSE_API UStartMessageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Message")
    void SetMessageText(FText InText);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StartTextBlock;

private:
    FText StoredMessage;
};
