#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMessageWidget.generated.h"

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
