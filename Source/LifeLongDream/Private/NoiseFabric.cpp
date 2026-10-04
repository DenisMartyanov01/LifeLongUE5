// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseFabric.h"


FastNoiseLite NoiseFabric::getBaseNoise(int seed, float frequency)
{
	FastNoiseLite noise;

	noise.SetSeed(seed);
	noise.SetFrequency(frequency);

	return noise;
}

FastNoiseLite NoiseFabric::getPerlinNoise(int seed = 12345, float frequency = 0.01)
{
	FastNoiseLite noise;

	noise.SetSeed(seed);
	noise.SetFrequency(frequency);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(5);
	noise.SetFractalLacunarity(2);
	noise.SetFractalGain(0.4f);
	noise.SetFractalWeightedStrength(-0.12f);

	return noise;
}

FastNoiseLite NoiseFabric::getCelluralValueNoise(int seed = 12345, float frequency = 0.01f)
{
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);

	noise.SetFractalType(FastNoiseLite::FractalType_None);

	noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
	noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
	noise.SetCellularJitter(1.5f);

	return noise;
}


FastNoiseLite NoiseFabric::getCelluralValueWarpNoise(int seed, float frequency)
{
	FastNoiseLite noise;

	noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);

	noise.SetFractalType(FastNoiseLite::FractalType_None);

	noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
	noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
	noise.SetCellularJitter(1.5f);

	noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	noise.SetDomainWarpAmp(135.0f);
	
	noise.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
	noise.SetFractalOctaves(6);
	noise.SetFractalLacunarity(2.25f);
	noise.SetFractalGain(0.9f);

	return noise;
}

FastNoiseLite NoiseFabric::getCaveNoise(int seed, float frequency)
{
	FastNoiseLite noise;

	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);

	noise.SetFractalType(FastNoiseLite::FractalType_FBm);

	noise.SetFractalOctaves(5);
	noise.SetFractalLacunarity(2.0f);
	noise.SetFractalGain(0.5f);
	noise.SetFractalWeightedStrength(-1.8f);

	return noise;
}

FastNoiseLite NoiseFabric::getRiverNoise(int seed, float frequency)
{
	FastNoiseLite noise;

	noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	noise.SetSeed(seed);
	noise.SetFrequency(frequency);

	noise.SetFractalType(FastNoiseLite::FractalType_None);

	noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
	noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);
	noise.SetCellularJitter(-1.12f);

	noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	noise.SetDomainWarpAmp(146);

	return noise;
}
