// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkData.h"
#include "BinaryMap.h"

#include "LayerData.generated.h"

UCLASS(Blueprintable)
class ULayerData : public UObject
{
	GENERATED_BODY()
public:	

	ULayerData() {};

	void createLayer(const BinaryMap& second, bool z);
	void createLayer(const ULayerData& second);

	UFUNCTION(BlueprintCallable, Category = "LayerData") void createLayer(int count);
	UFUNCTION(BlueprintCallable, Category = "LayerData") UChunkData* getChunk(int x, int y) const;
	UFUNCTION(BlueprintCallable, Category = "LayerData") void setChunk(UChunkData* chunk);

	UFUNCTION(BlueprintCallable, Category = "LayerData") FName getBlock(int x, int y, bool z) const;
	UFUNCTION(BlueprintCallable, Category = "LayerData") void setBlock(int x, int y, bool z, FName block);

	UFUNCTION(BlueprintCallable, Category = "LayerData") int getChunkCount() const { return chunkCount;}

	UFUNCTION(BlueprintCallable, Category = "LayerData") int getSize() const { return chunkCount * 16;}

	UFUNCTION(BlueprintCallable, Category = "LayerData") void connectLayers();

	void expand();
	
	int seek		(FVector c, int r, FName type) const;
	bool seekLine	(FVector a, FVector b, FName type) const;

	void fill		(FName type, bool z);
	void replace	(FName first, FName second);

	void rotate		(int side);
	void mirror		(bool side_x);

	void multiply		(const ULayerData* second, int x, int y, bool z1, bool z2);
	void multiplyByMask	(const ULayerData* second, const BinaryMap& mask, int x, int y, bool z);
	void multiplyByMask	(FName type, const BinaryMap& mask, int x, int y, bool z);
	void multiplyByMask	(TArray<FString> types, int seed, const BinaryMap& mask, int x, int y, bool z);
	void multiplyPoints	(const TArray<FVector2D>& points, bool z, FName type);
	void multiplyPoints	(const TArray<FVector>& points, FName type);

	TArray<UChunkData*> getData() {return chunks;}

private:
	TArray<UChunkData*> chunks;
	int chunkCount = 16;
};
