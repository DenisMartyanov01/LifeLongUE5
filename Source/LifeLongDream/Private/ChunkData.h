// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseType.h"

#include "ChunkData.generated.h"

UCLASS(Blueprintable)
class UChunkData : public UObject
{
public:
	GENERATED_BODY()

    UChunkData() {};

    UFUNCTION(BlueprintCallable, Category = "ChunkData") void setPosition(FVector2D newposition) { position = newposition; }
    UFUNCTION(BlueprintCallable, Category = "ChunkData") FVector2D getPosition() const { return position; }

    UFUNCTION(BlueprintCallable, Category = "ChunkData") void setType(FString newtype) {type = newtype;}
    UFUNCTION(BlueprintCallable, Category = "ChunkData") FString getType() const { return type; }

    UFUNCTION(BlueprintCallable, Category = "ChunkData") void setBiome(FString newbiome) { biome = newbiome; }
	UFUNCTION(BlueprintCallable, Category = "ChunkData") FString getBiome() const { return biome; }

	UFUNCTION(BlueprintCallable, Category = "ChunkData") int getSize() const {return 16;}

	UFUNCTION(BlueprintCallable, Category = "ChunkData") void setBlock(int x, int y, bool z, FName block);
	UFUNCTION(BlueprintCallable, Category = "ChunkData") FName getBlock(int x, int y, bool z) const;
    UFUNCTION(BlueprintCallable, Category = "ChunkData") void removeBlock(int x, int y, bool z);

    void addRow(FName name, TArray<TPair<int, int>> nums);
    TArray<TPair<int, int>> getRow(FName name) const;
    UFUNCTION(BlueprintCallable, Category = "ChunkData") TArray<FName> getBlocks() const;

    UFUNCTION(BlueprintCallable, Category = "ChunkData") bool contains(FName type) const;

    const TMap<FName, TArray<TPair<int, int>>> * getData() const { return &data; }
    void setData( TMap<FName, TArray<TPair<int, int>>> data2) {data.Reset(); data = data2;}

    void set(const UChunkData& second);

    //void fill(FString type);

    UFUNCTION(BlueprintCallable, Category = "ChunkData") void print() const;

private:
    TMap<FName, TArray<TPair<int, int>>> data;
    FVector2D position = {0, 0};
    FString type = "MTA";
    FString biome = "None";
};
