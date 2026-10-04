// Fill out your copyright notice in the Description page of Project Settings.

#include "LayerData.h"
#include "Math/RandomStream.h"
#include "CoordsConvert.h"

void ULayerData::createLayer(const BinaryMap& map, bool z)
{
	chunks.Empty();

	chunkCount = map.getSize() / 16;

	for(int x = 0; x < chunkCount; x++)
	{
		for(int y = 0; y < chunkCount; y++)
		{
			UChunkData* ch = NewObject<UChunkData>();
			ch->setPosition(FVector2D(x, y));
			chunks.Emplace(ch);
		}
	}

	for(int x = 0; x < map.getSize(); x++)
	{
		for(int y = 0; y < map.getSize(); y++)
		{
			setBlock(x, y, z, map.get(x, y) ? "temp" : "air");
		}
	}
}

void ULayerData::createLayer(const ULayerData& second)
{
	chunks.Empty();

	chunkCount = second.getChunkCount();

	for(int x = 0; x < chunkCount; x++)
	{
		for(int y = 0; y < chunkCount; y++)
		{
			UChunkData* ch = NewObject<UChunkData>();
			ch->setPosition(FVector2D(x, y));
			chunks.Emplace(ch);
		}
	}

	for(int x = 0; x < second.getSize(); x++)
	{
		for(int y = 0; y < second.getSize(); y++)
		{
			setBlock(x, y, 0, second.getBlock(x, y, 0));
			setBlock(x, y, 1, second.getBlock(x, y, 1));
		}
	}

}

void ULayerData::createLayer(int count = 16)
{
	chunks.Empty();

	chunkCount = count;

	for(int x = 0; x < count; x++)
	{
		for(int y = 0; y < count; y++)
		{
			UChunkData* ch = NewObject<UChunkData>();
			ch->setPosition(FVector2D(x, y));
			chunks.Emplace(ch);
		}
	}
}

UChunkData* ULayerData::getChunk(int x, int y) const
{
	if (x < 0 || y < 0 || x >= chunkCount || y >= chunkCount) return nullptr;

	return chunks[y * chunkCount + x];
}

void ULayerData::setChunk(UChunkData* chunk)
{
	UChunkData* a = getChunk(chunk->getPosition().X, chunk->getPosition().Y);

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			a->setBlock(i, j, 0, chunk->getBlock(i, j, 0));
			a->setBlock(i, j, 1, chunk->getBlock(i, j, 1));
		}
	}
}


FName ULayerData::getBlock(int x, int y, bool z) const
{
	if (x < 0 || y < 0 || x >= chunkCount * 16 || y >= chunkCount * 16) return "air";
	
	return chunks[(x / 16) + (y / 16) * chunkCount]->getBlock(x % 16, y % 16, z);
}

void ULayerData::setBlock(int x, int y, bool z, FName block)
{
	if (x < 0 || y < 0 || x >= chunkCount * 16 || y >= chunkCount * 16) return;

	getChunk(x / 16, y / 16)->setBlock(x % 16, y % 16, z, block);
}


//Private

void ULayerData::connectLayers()
{
	for (int i = 0; i < getSize(); i++)
	{
		for (int j = 0; j < getSize(); j++)
		{
			if (getBlock(i, j, 1) != "air") 
			{
				setBlock(i, j, 0, getBlock(i,j, 1));
				setBlock(i, j, 1, "air");
			}
		}
	}
}

void ULayerData::expand()
{
	ULayerData* new_mat = NewObject<ULayerData>();
	new_mat->createLayer(chunkCount * 2);

	for (int i = 0; i < chunkCount * 16; i++) 
	{ 
		for (int j = 0; j < chunkCount * 16; j++) 
		{
			for(int z = 0; z <= 1; z++) 
			{
				new_mat->setBlock(i * 2,		j * 2,			(bool)z, getBlock(i, j, (bool)z));
				new_mat->setBlock(i * 2 + 1,	j * 2,			(bool)z, getBlock(i, j, (bool)z));
				new_mat->setBlock(i * 2,		j * 2 + 1,		(bool)z, getBlock(i, j, (bool)z));
				new_mat->setBlock(i * 2 + 1,	j * 2 + 1,		(bool)z, getBlock(i, j, (bool)z));
			}
		}
	}

	chunks.Empty();

	for (auto i : new_mat->getData())
	{
		chunks.Add (i);
	}
	chunkCount = new_mat->chunkCount;

}

int ULayerData::seek(FVector c, int r, FName type) const
{
	int result = 0;

	for (int i = -r; i <= r; i++) 
	{
		if (i + c.X < 0 || i + c.X >= getSize()) continue; 
		for (int j = -r; j <= r; j++) 
		{
			if (j + c.Y < 0 && j + c.Y >= getSize()) continue;

			if (i * i + j * j <= r * r) 
			{
				if (getBlock(c.X + i, c.Y + j, c.Z == 1) == type)
					result++;
			}
		}
	}
	return result;
}

bool ULayerData::seekLine(FVector a, FVector b, FName type) const
{
	int lenY = b.Y - a.Y;
	int lenX = b.X - a.X;

	int maxLen = (std::max)(abs(lenY), abs(lenX));
	if (!maxLen) maxLen = 1;

	for (int i = 0; i <= maxLen; i++) 
	{
		if (getBlock(a.X + i * lenX / maxLen, a.Y + i * lenY / maxLen, (bool)a.Z) == type) 
			return true;
	}
	return false;
}

void ULayerData::fill(FName type, bool z)
{
	for (int i = 0; i < getSize(); i++) 
	{
		for (int j = 0; j < getSize(); j++) 
		{
			setBlock(i, j, z, type);
		}
	}
}

void ULayerData::replace(FName first, FName second)
{
	for (int i = 0; i < getSize(); i++) 
	{
		for (int j = 0; j < getSize(); j++) 
		{
			for (int z = 0; z <= 1; z++) 
			{
				if (getBlock(i, j, (bool)z) == first) setBlock(i, j, (bool)z, second);
			}
		}
	}
}

void ULayerData::rotate(int side)
{
	if (side % 4 == 0) return;

	ULayerData * second = NewObject<ULayerData>();
	second->createLayer(*this);

	for (int i = 0; i < getSize(); i++)
	{
		for (int j = 0; j < getSize(); j++)
		{
			if (side % 4 == 1) 
			{
				this->setBlock(i, j, 0, second->getBlock(j, getSize() - i - 1, 0));
				this->setBlock(i, j, 1, second->getBlock(j, getSize() - i - 1, 1));
			}
			else if (side % 4 == 2) 
			{
				this->setBlock(i, j, 0, second->getBlock(getSize() - i - 1, getSize() - j - 1, 0));
				this->setBlock(i, j, 1, second->getBlock(getSize() - i - 1, getSize() - j - 1, 1));
			}
			else if (side % 4 == 3) 
			{
				this->setBlock(i, j, 0, second->getBlock(getSize() - j - 1, i, 0));
				this->setBlock(i, j, 1, second->getBlock(getSize() - j - 1, i, 1));
			}
		}
	}
	
}

void ULayerData::mirror(bool side_x)
{
	ULayerData * second = NewObject<ULayerData>();
	second->createLayer(*this);

	for (int i = 0; i < getSize(); i++)
	{
		for (int j = 0; j < getSize(); j++)
		{
			if (side_x)
			{
				this->setBlock(i, j, 0, second->getBlock(getSize() - i - 1, j, 0));
				this->setBlock(i, j, 1, second->getBlock(getSize() - i - 1, j, 1));
			}
			else 
			{
				this->setBlock(i, j, 0, second->getBlock(i, getSize() - j - 1, 0));
				this->setBlock(i, j, 1, second->getBlock(i, getSize() - j - 1, 1));
			}
		}
	}

}

void ULayerData::multiply(const ULayerData* second, int x, int y, bool z1, bool z2)
{
	for (int i = 0; i < second->getSize(); i++) 
	{
		for (int j = 0; j < second->getSize(); j++) 
		{
			if (second->getBlock(i, j, z2) != "temp")
				setBlock(i + x, j + y, z1, second->getBlock(i, j, z2));
		}
	}
}

void ULayerData::multiplyByMask(const ULayerData* second, const BinaryMap& map, int x, int y, bool z)
{
	for (int i = 0; i < second->getSize(); i++) 
	{
		for (int j = 0; j < second->getSize(); j++) 
		{
			if (map.get(i, j))
				setBlock(x + i, y + j, z, second->getBlock(i, j, z));
		}
	}
}

void ULayerData::multiplyByMask(FName type, const BinaryMap& mask, int x, int y, bool z)
{
	for (int i = 0; i < mask.getSize(); i++) 
	{
		for (int j = 0; j < mask.getSize(); j++) 
		{
			if (mask.get(i, j))
				setBlock(x + i, y + j, z, type);
		}
	}
}

void ULayerData::multiplyByMask(TArray<FString> types, int seed, const BinaryMap& mask, int x, int y, bool z)
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed);

	for (int i = 0; i < mask.getSize(); i++) 
	{
		for (int j = 0; j < mask.getSize(); j++) 
		{
			if (mask.get(i, j))
				setBlock(x + i, y + j, z, FName(types[RandomStream.FRandRange(0, types.Num())]));
		}
	}
}

void ULayerData::multiplyPoints(const TArray<FVector2D>& points, bool z, FName type)
{
	for (int i = 0; i < points.Num(); i++)
	{
		setBlock(points[i].X, points[i].Y, z, type);
	}
}

void ULayerData::multiplyPoints(const TArray<FVector>& points, FName type)
{
	for (int i = 0; i < points.Num(); i++)
	{
		setBlock(points[i].X, points[i].Y, points[i].Z != 0, type);
	}
}


//UE_LOG(LogTemp, Warning, TEXT("%i"), RandomStream.RandRange(0, 1));
