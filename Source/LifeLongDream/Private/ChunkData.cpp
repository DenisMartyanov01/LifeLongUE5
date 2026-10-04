#include "ChunkData.h"
#include "CoordsConvert.h"


void UChunkData::setBlock(int x, int y, bool z, FName block)
{
	if (x < 0 || y < 0 || x >= 16 || y >= 16) return;

	removeBlock(x, y, z);
	
	int index = UCoordsConvert::XYToIndex(x, y, z, 16);

	if (block == "air") return;

	auto connect = [&](TPair<int, int>& val) 
	{
		for (auto seek_pair : data[block])
		{
			if (seek_pair == val) continue;

			if (val.Key + 1 == seek_pair.Value)
			{
				val.Key = seek_pair.Key;
				data[block].Remove(seek_pair);
				return;
			}
			else if (val.Value + 1 == seek_pair.Key)
			{
				val.Value = seek_pair.Value;
				data[block].Remove(seek_pair);
				return;
			}
		}
	};

	if (data.Contains(block))
	{
		for (auto& pair : data[block])
		{
			if (pair.Key == index + 1)
			{
				pair.Key = index;
				connect(pair);
				return;
			}
			else if (pair.Value == index - 1)
			{
				pair.Value = index;
				connect(pair);
				return;
			}
		}
		data[block].Add({index, index});
	}
	else
		data.Add(block, {{index, index}});
}


FName UChunkData::getBlock(int x, int y, bool z) const
{
	if (x < 0 || y < 0 || x >= 16 || y >= 16) return "air";
	
	int index = UCoordsConvert::XYToIndex(x, y, z, 16);

	for(auto& row : data)
	{
		for (auto i : row.Value)
		{
			if (i.Key <= index && index <= i.Value)
			{
				return row.Key;
			}
		}
	}

	return "air";
}

void UChunkData::removeBlock(int x, int y, bool z)
{
	FName curBlock = getBlock(x, y, z);

	if (curBlock == "air") return;

	TPair<int, int> n;
	bool delete_this = false;
	bool add = false;

	for (auto& pair : data[curBlock])
	{
		if (pair.Key <= UCoordsConvert::XYToIndex(x, y, z, 16) && UCoordsConvert::XYToIndex(x, y, z, 16) <= pair.Value)
		{
			if (pair.Key == pair.Value) 
			{	
				n = pair;
				delete_this = true;
				break;
			}

			if (pair.Key == UCoordsConvert::XYToIndex(x, y, z, 16))
				pair.Key++;
			else if (pair.Value == UCoordsConvert::XYToIndex(x, y, z, 16))
				pair.Value--;
			else
			{
				add = true;
				n = {UCoordsConvert::XYToIndex(x, y, z, 16) + 1, pair.Value};
				pair.Value = UCoordsConvert::XYToIndex(x, y, z, 16) - 1;
			}
			break;
		}
	
	}
	if (delete_this)
		data[curBlock].Remove(n);
	if (add) 
		data[curBlock].Add(n);

}

void UChunkData::set(const UChunkData& second)
{
	setData(*second.getData());
	position = second.getPosition();
	type = second.getType();
	biome = second.getBiome();
}

void UChunkData::print() const
{
	for (const auto& row : data)
    {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *row.Key.ToString());

        for (const TPair<int32, int32>& pair : row.Value)
        {
			UE_LOG(LogTemp, Warning, TEXT("[ %i : %i ]"), pair.Key, pair.Value);
        }
    }
}


void UChunkData::addRow(FName name, TArray<TPair<int, int>> nums)
{
	if (data.Find(name))
	{
		for(auto i : nums)
			data[name].AddUnique(i);
		return;
	}
	data.Add(name, nums);
}

TArray<TPair<int, int>> UChunkData::getRow(FName name) const
{
	if (data.Find(name))
		return data[name];
	
	return {};
}

TArray<FName> UChunkData::getBlocks() const
{
	TArray<FName> result;

	data.GetKeys(result);
	return result;

}

bool UChunkData::contains(FName block) const
{
	return data.Contains(block);
}

