#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndTurnWidget.generated.h"

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
