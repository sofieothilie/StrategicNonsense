#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UnitActor.h"
#include "GameStatusWidget.generated.h"

UCLASS()
class STRATEGICNONSENSE_API UGameStatusWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetTeamInfo(bool bIsTeam1, const FString& Colour, bool bIsAI);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetUnitHealth(bool bIsTeam1, EGameUnitType UnitType, int32 CurrentHP, int32 MaxHP);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetTurnText(const FString& TurnDescription);


protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team1Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team1SniperText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team1BrawlerText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team2Text;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team2SniperText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Team2BrawlerText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Turntext;
};

