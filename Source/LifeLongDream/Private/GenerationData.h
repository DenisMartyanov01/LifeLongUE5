// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LayerData.h"
#include "NoiseFabric.h"
#include "CoreMinimal.h"


class Structure
{
public:
	FString method;
	TArray<FString> walls;
	TArray<FString> floor;
	TArray<FString> objects;
	int room_count;
	int room_size;
	int structure_size;
};

class Biome
{
public:

	FastNoiseLite noise;
	TArray<TPair<TArray<FString>, float>> trees;
	TArray<FString> ores;
	TArray<FString> blocks;
	TArray<FString> structures;
};


class BiomesData
{
public:
	BiomesData() {};

	bool is_valid = false;

	void setSeed(int seed);

	TMap<FString, Biome> biomes;

};

class StructuresData
{
public:
	StructuresData() {};
	
	bool is_valid = false;

	TMap<FString, Structure> structures;

};

class HeightData
{
public:
	HeightData() {};

	bool is_valid = false;

	TMap<FString, float> height;
	TMap<FString, float> biomes;

};