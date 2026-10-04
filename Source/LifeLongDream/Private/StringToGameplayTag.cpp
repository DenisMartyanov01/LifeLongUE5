#include "StringToGameplayTag.h"
#include "GameplayTagContainer.h"

FGameplayTag UStringToGameplayTag::Conv_StringToGameplayTag(const FString& TagString)
{
	return FGameplayTag::RequestGameplayTag(FName(*TagString));
}
