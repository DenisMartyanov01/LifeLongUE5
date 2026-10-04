// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"

class TEST_API NoiseFabric
{
public:

	static FastNoiseLite getBaseNoise(int seed, float frequency);

	static FastNoiseLite getPerlinNoise(int seed, float frequency);

	static FastNoiseLite getCelluralValueNoise(int seed, float frequency);

	static FastNoiseLite getCelluralValueWarpNoise(int seed, float frequency);

	static FastNoiseLite getCaveNoise(int seed, float frequency);

	static FastNoiseLite getRiverNoise(int seed, float frequency);

};
