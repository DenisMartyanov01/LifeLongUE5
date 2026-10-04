// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseType.generated.h"

UENUM(BlueprintType)
enum E_BaseType
{
	Magic					UMETA(DisplayName = "Magic"),
	Technology				UMETA(DisplayName = "Technology"),
	Adventure				UMETA(DisplayName = "Adventure"),
	
	MTA						UMETA(DisplayName = "MTA"),
	Magic_Technology		UMETA(DisplayName = "Magic_Technology"),
	Magic_Adventure			UMETA(DisplayName = "Magic_Adventure"),
	Technology_Adventure	UMETA(DisplayName = "Technology_Adventure")

};

UENUM(BlueprintType)
enum E_TimeOfYear
{
	Spring					UMETA(DisplayName = "Spring"),
	Summer					UMETA(DisplayName = "Summer"),
	Autumn					UMETA(DisplayName = "Autumn"),
	Winter					UMETA(DisplayName = "Winter")
};


UENUM(BlueprintType)
enum E_BiomeOverWorld
{
	None					UMETA(DisplayName = "None"),
	Forest					UMETA(DisplayName = "Forest"),
	DeepForest				UMETA(DisplayName = "DeepForest"),
	Jungle					UMETA(DisplayName = "Jungle"),
	Field					UMETA(DisplayName = "Field"),
	Desert					UMETA(DisplayName = "Desert"),
	Clif					UMETA(DisplayName = "Clif"),
	Ocean					UMETA(DisplayName = "Ocean"),
	Snow					UMETA(DisplayName = "Snow")
};

UENUM(BlueprintType)
enum E_Layer
{
	test					UMETA(DisplayName = "test"),

	over_world				UMETA(DisplayName = "over_world"),
	cave_1					UMETA(DisplayName = "cave_1"),
	cave_2					UMETA(DisplayName = "cave_2"),
	cave_3					UMETA(DisplayName = "cave_3")
};



