#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "StringToGameplayTag.generated.h"

UCLASS()
class TEST_API UStringToGameplayTag : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (DisplayName = "String To Gameplay Tag"))
	static FGameplayTag Conv_StringToGameplayTag(const FString& TagString);

};