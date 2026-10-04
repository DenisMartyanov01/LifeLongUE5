// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "LayerData.h"
#include "MyGameInstance.h"
#include "GenerationTools.h"

#include "WorldGen.generated.h"

UCLASS(Blueprintable)
class UWorldGen : public UObject, public GenerationTools
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintCallable, Category = "Generation") void setSeed(int s) {seed = s;}
	UFUNCTION(BlueprintCallable, Category = "Generation") void setSector(FVector2D v) {sector = v;}
	UFUNCTION(BlueprintCallable, Category = "Generation") FVector2D getSector() const {return sector;}


	UFUNCTION(BlueprintCallable, Category = "Generation") ULayerData* GenLayer(int size, E_Layer name);
	UFUNCTION(BlueprintCallable, Category = "Generation") void TestNoise(int size, float frequency, bool log);
	ULayerData* GenOverWorld(int size);
	ULayerData* GenCave(int size, E_Layer name);
	ULayerData* TestLayerFunctions();

	void setHeight		(ULayerData* layer, E_Layer name);
    void setBiomes		(ULayerData* layer, E_Layer name);
    void setOres		(ULayerData* layer, E_Layer name);
    void setBlocks		(ULayerData* layer, E_Layer name);
    void setStructures	(ULayerData* layer, E_Layer name);
	void setRiver		(ULayerData* layer, int r);

	int seed = 12345;
	FVector2D sector = {0, 0};
};
