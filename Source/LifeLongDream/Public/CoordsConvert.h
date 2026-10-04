// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoordsConvert.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API UCoordsConvert : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static int XYToIndex(int x, int y, bool z, int size);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector IndexToXY(int i, int size);
	

	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector CoordsToBlock(float x, float y, bool z);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector BlockToCoords(float x, float y, bool z);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector BlockToChunk(float x, float y, bool z);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector ChunkToBlock(float x, float y, bool z);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector CoordsToChunk(float x, float y, bool z);
	
	UFUNCTION(BlueprintPure, Category = "CoordsConvert")
	static FVector ChunkToCoords(float x, float y, bool z);
};
