// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGen.h"
#include "SaveLoad.h"
#include "GenerationData.h"

#include "NoiseFabric.h"


ULayerData* UWorldGen::GenLayer(int size, E_Layer name)
{
	switch(name)
	{
	case E_Layer::test:
		return TestLayerFunctions();
	case E_Layer::over_world:
		return GenOverWorld(size);
	default:
		return GenCave(size, name);
	}
}

ULayerData* UWorldGen::GenOverWorld(int size)
{
	ULayerData* layer = NewObject<ULayerData>();
	layer->createLayer(size);
	
	setHeight(layer, E_Layer::over_world);
	setBlocks(layer, E_Layer::over_world);
	setBiomes(layer, E_Layer::over_world);
	setStructures(layer, E_Layer::over_world);

	return layer;
}

ULayerData* UWorldGen::GenCave(int size, E_Layer name)
{
	ULayerData* layer = NewObject<ULayerData>();
	layer->createLayer(size);


	setBlocks	(layer, name);
	setBiomes	(layer, name);
	setOres		(layer, name);


	FastNoiseLite caveNoise	= NoiseFabric::getCaveNoise(seed, 0.008f);

	for (float i = 0; i < layer->getSize(); i++)
	{
		for (float j = 0; j < layer->getSize(); j++)
		{
			float cave_n = ((caveNoise.GetNoise(i + sector.X * size * 16, j + sector.Y * size * 16) + 0.3) / 6) * 10;
			if (cave_n < 0.3) 
			{
				layer->setBlock(i, j, 1, "air");
				continue;
			}
		}
	}


	setHeight(layer, name);


	TArray<FVector> connected;
	for (float i = 0; i < layer->getSize(); i++)
	{
		for (float j = 0; j < layer->getSize(); j++)
		{
			if (layer->getBlock(i, j, 1) == "air" && layer->seek({(float)i, (float)j, 1}, 1, "water"))
			{
				seekConnected(layer, {(float)i, (float)j, 1}, connected);
				layer->multiplyPoints(connected, "water");
				connected.Empty();
			}
		}
	}


	setStructures(layer, name);


	return layer;
}

ULayerData* UWorldGen::TestLayerFunctions()
{
	FString f = UEnum::GetValueAsString(E_Layer::over_world);

	UE_LOG(LogTemp, Log, TEXT("%s"), *f);

	ULayerData* layer = NewObject<ULayerData>();
	layer->createLayer(1);

	return layer;
}


void UWorldGen::setHeight		(ULayerData* layer, E_Layer name)
{
	FastNoiseLite heightNoise =	NoiseFabric::getPerlinNoise(seed, 0.001f);
	HeightData heighData = USaveLoad::LoadHeight(name);

	if (!heighData.is_valid)
    {
		UE_LOG(LogTemp, Log, TEXT("Can't setHeight, can't load height")); 
        return;
    }

	for (float i = 0; i < layer->getSize(); i++)
	{
		for (float j = 0; j < layer->getSize(); j++)
		{
			float n = ((heightNoise.GetNoise(i + sector.X * layer->getSize(), j + sector.Y * layer->getSize()) + 1) / 2);
			
			UE_LOG(LogTemp, Log, TEXT("%f"), n); 

			for (auto& height : heighData.height)
			{
				if (n < height.Value)
				{
					layer->setBlock(i, j, 0, FName(height.Key));
					if (name > E_Layer::over_world)
						layer->setBlock(i, j, 1, FName(height.Key));
					break;
				}
			}
		}
	}

}

void UWorldGen::setBiomes		(ULayerData* layer, E_Layer name)
{
	FastNoiseLite biomeNoise = NoiseFabric::getCelluralValueWarpNoise(seed, 0.005f);
	HeightData heighData = USaveLoad::LoadHeight(name);

	if (!heighData.is_valid)
    {
		UE_LOG(LogTemp, Log, TEXT("Can't setHeight, can't load height")); 
        return;
    }

	for (int i = 0; i < layer->getChunkCount(); i++)
	{
		for (int j = 0; j < layer->getChunkCount(); j++) 
		{
			float x = i * 16;
			float y = j * 16;

			biomeNoise.DomainWarp(x, y);
			float n = (biomeNoise.GetNoise(x  + sector.X * layer->getSize(), y + sector.Y * layer->getSize()) + 1) / 2;

			if (layer->getChunk(i, j)->getRow("water").Num() >= 64)
			{	
				layer->getChunk(i, j)->setBiome("Ocean");
			}
			else
			{
				for (auto& biome : heighData.biomes )
				{
					if (n < biome.Value)
					{
						layer->getChunk(i, j)->setBiome(biome.Key);
						break;
					}
				}
			}
		}
	}
}

void UWorldGen::setOres			(ULayerData* layer, E_Layer name)
{
	BiomesData biomesData = USaveLoad::LoadBiomes();
    FString layerStr = UEnum::GetValueAsString(name);
	int oreCount = layer->getSize();

	if (!biomesData.is_valid)
    {
		UE_LOG(LogTemp, Log, TEXT("Can't setOres, can't load biomes")); 
        return;
    }
	FRandomStream RandomStream;
	RandomStream.Initialize(GetTypeHash(layerStr + "_ores" + FString::FromInt(seed) + sector.ToString()));

	TArray<FVector2D> ores = genPoints
	(
		GetTypeHash(layerStr + FString::FromInt(seed) + sector.ToString()), 
		layer->getSize() - 1, 
		layer->getSize() - 1, 
		oreCount
	);

	for (const FVector2D& p : ores ) 
	{
		FString curBiome = layer->getChunk((int)p.X / 16, (int)p.Y / 16)->getBiome();

		if (curBiome == "None") continue;
		if (biomesData.biomes[curBiome].ores.Num() == 0) continue;

		layer->multiplyPoints
		(
			randomWalk(GetTypeHash(layerStr + "_ore" + FString::FromInt(seed) + p.ToString()), p, 20), 
			0, 
			FName(biomesData.biomes[curBiome].ores[RandomStream.RandRange(0, biomesData.biomes[curBiome].ores.Num() - 1)])
		);
		layer->multiplyPoints
		(
			randomWalk(GetTypeHash(layerStr + "_ore_1" + FString::FromInt(seed) + p.ToString()), p, 20), 
			1, 
			FName(biomesData.biomes[curBiome].ores[RandomStream.RandRange(0, biomesData.biomes[curBiome].ores.Num() - 1)])
		);
	}

}

void UWorldGen::setBlocks		(ULayerData* layer, E_Layer name)
{
	FastNoiseLite biomeNoise = NoiseFabric::getCelluralValueWarpNoise(seed, 0.005f);
	FastNoiseLite treeNoise	= NoiseFabric::getBaseNoise(seed, 1);
	HeightData heighData  = USaveLoad::LoadHeight(name);
	BiomesData biomesData = USaveLoad::LoadBiomes();

	if (!heighData.is_valid)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't setBlocks, can't load heights")); 
        return;
	}
	if (!biomesData.is_valid)
    {
		UE_LOG(LogTemp, Log, TEXT("Can't setOres, can't load biomes")); 
        return;
    }

    FString layerStr = UEnum::GetValueAsString(name);

	FRandomStream RandomStream;
	RandomStream.Initialize(GetTypeHash(layerStr + "_trees" + FString::FromInt(seed) + sector.ToString()));

	for (int i = 0; i < layer->getSize(); i++)
	{
		for (int j = 0; j < layer->getSize(); j++) 
		{
			if (layer->getBlock(i, j, 0) != "air") continue;

			float x = i + sector.X * layer->getSize();
			float y = j + sector.Y * layer->getSize();

			biomeNoise.DomainWarp(x, y);
			float n = (biomeNoise.GetNoise(x, y) + 1) / 2;

			x = i + sector.X * layer->getSize();
			y = j + sector.Y * layer->getSize();

			Biome biome;

			for (auto& biomeHeight : heighData.biomes )
			{
				if (n < biomeHeight.Value)
				{
					if (biomesData.biomes.Contains(biomeHeight.Key))
						biome = biomesData.biomes[biomeHeight.Key];
					break;
				}
			}
				
			layer->setBlock(i, j, 0, FName(biome.blocks[(int)((biome.noise.GetNoise(x, y) + 1) / 2 * biome.blocks.Num())]));
			if (name > E_Layer::over_world)
			{
				layer->setBlock(i, j, 1, FName(biome.blocks[(int)((biome.noise.GetNoise(x, y) + 1) / 2 * biome.blocks.Num())]));
			}

			// Trees

			if (biome.trees.Num() == 0) continue;

			float tree_n = (treeNoise.GetNoise(i + sector.X * layer->getSize(), j + sector.Y * layer->getSize()) + 1) / 2;

			for (const auto& treeLevel : biome.trees)
			{
				if (tree_n < treeLevel.Value) 
				{
					layer->setBlock(i, j, 1, FName(treeLevel.Key[RandomStream.FRandRange(0, treeLevel.Key.Num() - 1)]));
					break;
				}
			}
		}
	}
}

void UWorldGen::setStructures	(ULayerData* layer, E_Layer name)
{
	StructuresData structuresData = USaveLoad::LoadStructures();
	BiomesData biomesData = USaveLoad::LoadBiomes();
	biomesData.setSeed(seed);

	if (!structuresData.is_valid)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't setStructures, can't load structures")); 
        return;
	}
	if (!biomesData.is_valid)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't setStructures, can't load biomes")); 
        return;
	}

    FString layerStr = UEnum::GetValueAsString(name);

	FRandomStream RandomStream;
	RandomStream.Initialize(GetTypeHash(layerStr + "_structures" + FString::FromInt(seed) + sector.ToString()));

	int roadLen = 60;

	int structureCount = 8;
	int newSeed = seed;

	TArray<FVector2D> points = genPoints
	(
		GetTypeHash(layerStr + "_structures" + FString::FromInt(seed) + sector.ToString()),
		layer->getSize() - 1,
		layer->getSize() - 1,
		structureCount
	);
	
	for (const FVector2D& p : points)
	{
		if (! biomesData.biomes.Contains(layer->getChunk(p.X / 16, p.Y / 16)->getBiome()))
		{
			UE_LOG(LogTemp, Log, TEXT("Can't load existing biome"));
			continue;
		}
		
		Biome curBiome = biomesData.biomes[layer->getChunk(p.X / 16, p.Y / 16)->getBiome()];
		if (curBiome.structures.Num() == 0) continue; 

		if (! structuresData.structures.Contains(curBiome.structures[RandomStream.RandRange(0, curBiome.structures.Num() - 1)]))
		{
			UE_LOG(LogTemp, Log, TEXT("Can't load existing structure"));
			continue;
		}

		Structure Struct = structuresData.structures[curBiome.structures[RandomStream.RandRange(0, curBiome.structures.Num() - 1)]];

		if (p.X > layer->getSize() - Struct.structure_size || p.Y > layer->getSize() - Struct.structure_size) continue;

		if (Struct.method == "BSP")
		{
			BinaryMap* dungeonMap = new BinaryMap(Struct.structure_size);

			for (const auto& i : BSP
			(
				{{0, 0}, 
				{(float)Struct.structure_size, (float)Struct.structure_size}}, 
				newSeed++, 
				Struct.room_size, 
				Struct.room_size
			))
			{
				dungeonMap->multiplyPoints(genRectangle(i.pos, i.pos + i.size), true);			
				dungeonMap->multiplyPoints(genRectangle(i.pos + 1, i.pos + i.size - 1), false);			
			}
			layer->multiplyByMask(Struct.walls, seed, *dungeonMap, p.X, p.Y, 1);
			layer->multiplyByMask(Struct.floor, seed, *dungeonMap, p.X, p.Y, 0);
			dungeonMap->invert();
			layer->multiplyByMask("air", *dungeonMap, p.X, p.Y, 1);
			layer->multiplyByMask(Struct.floor, seed, *dungeonMap, p.X, p.Y, 0);
				
			delete dungeonMap;
			
		}
		else if (Struct.method == "dungeonWithRoads")
		{
			BinaryMap* dungeonMap = new BinaryMap(Struct.structure_size);

			TArray<TArray<FVector2D>> allPoints = genFigures(newSeed++, {(float)Struct.structure_size, (float)Struct.structure_size}, Struct.room_count, 0, 1, {rectangle}, Struct.room_size, Struct.room_size, 7);
				
			TArray<FVector2D> centers;
			for (const auto& p2 : allPoints)
			{
				centers.Add(getCenter(p2));
			}
	
			for (int i = 0; i < centers.Num() - 1; i++)
			{
				for (int j = 0; j < centers.Num(); j++)
				{
					if (FVector::Dist({centers[i].X, centers[i].Y, 0}, {centers[j].X, centers[j].Y, 0}) < roadLen )
						allPoints.Add(genRoads(centers[i], centers[j], 2));
				}
			}

			for (auto& roomPoints : allPoints)
			{
				dungeonMap->multiplyPoints(roomPoints, true);
			}

			layer->multiplyByMask(Struct.walls, newSeed++, *dungeonMap, p.X, p.Y, 1);
			layer->multiplyByMask(Struct.floor, newSeed++,  *dungeonMap, p.X, p.Y, 0);
			dungeonMap->smooth();
			layer->multiplyByMask("air", *dungeonMap, p.X, p.Y, 1);

			for (auto& roomPoints : allPoints)
			{
				roomPoints.Empty();
			}
			delete dungeonMap;
			
			allPoints.Empty();
			centers.Empty();

		}
		else if (Struct.method == "dungeonConnect")
		{
			BinaryMap* dungeonMap = new BinaryMap(Struct.structure_size);

			TArray<TArray<FVector2D>> allPoints = genFigures(newSeed++, {(float)Struct.structure_size, (float)Struct.structure_size}, Struct.room_count, 1, 0, {rectangle}, Struct.room_size, Struct.room_size, 7);
			
			for (auto& roomPoints : allPoints)
			{
				dungeonMap->multiplyPoints(roomPoints, true);
			}

			layer->multiplyByMask(Struct.walls, newSeed++, *dungeonMap, p.X, p.Y, 1);
			layer->multiplyByMask(Struct.floor, newSeed++, *dungeonMap, p.X, p.Y, 0);
			dungeonMap->smooth();
			layer->multiplyByMask("air", *dungeonMap, p.X, p.Y, 1);
				
			for (auto& roomPoints : allPoints)
				roomPoints.Empty();
			allPoints.Empty();

			delete dungeonMap;
			
		}

		if (Struct.objects.Num() == 0) continue;

		bool s; FString msg;

		TArray<FVector2D> objPoints = genPoints
		(
			GetTypeHash(layerStr + "_objects" + FString::FromInt(seed) + sector.ToString()), 
			Struct.structure_size - 1, 
			Struct.structure_size - 1, 
			Struct.room_count * 4
		);

		for(const auto& obj_p : objPoints)
		{
			ULayerData* obj = USaveLoad::LoadAsset(Struct.objects[RandomStream.RandRange(0, Struct.objects.Num() - 1)], s, msg);
			if (!obj) continue;

			if (RandomStream.RandRange(0, 3) == 0) obj->rotate(RandomStream.RandRange(0, 3));
			if (RandomStream.RandRange(0, 3) == 0) obj->mirror(RandomStream.RandRange(0, 1) > 0 );

			layer->multiply(obj, p.X + obj_p.X, p.Y + obj_p.Y, 0, 0);
			layer->multiply(obj, p.X + obj_p.X, p.Y + obj_p.Y, 1, 1);
		}
	}
}

void UWorldGen::setRiver(ULayerData* layer, int r) 
{
	
}


void UWorldGen::TestNoise(int size, float frequency, bool log)
{
	FastNoiseLite noise = NoiseFabric::getCelluralValueWarpNoise(seed, frequency);

	noise.SetFractalGain(0);

	TArray<FColor> pixels;

	for(float i = 0; i < size; i++) 
	{
		for(float j = 0; j < size; j++)
		{
			float x = i;
			float y = j;

			noise.DomainWarp(x, y);
			float val = noise.GetNoise(x, y);
			
			if (log) UE_LOG(LogTemp, Log, TEXT("%f"), val);

			uint8 n = 0;

			for (int k = -1; k < 2; k++)
				for (int n1 = -1; n1 < 2; n1++)
					if (val != noise.GetNoise(x + k, y + n1)) { n = 255; break; }

			pixels.Add(FColor{ n, n, n, 0 });
		}
	}

	bool s;
	FString m;

	USaveLoad::SaveArrayToPNG(pixels, "asd", size, s, m );
}
