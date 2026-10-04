// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerationData.h"


void BiomesData::setSeed(int seed)
{
	for (auto& i : biomes)
		i.Value.noise.SetSeed(seed);
}
