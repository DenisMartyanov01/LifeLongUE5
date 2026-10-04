// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerationTools.h"
#include <bitset>

#include "CoordsConvert.h"


void GenerationTools::spread(BinaryMap& map, int seed, int r) 
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	if (r <= 0) return;
	
	BinaryMap map2(map.getSize());

	for (int i = 0; i < map.getSize(); i++) 
	{
		for (int j = 0; j < map.getSize(); j++) 
		{
			for (int k = 1; k <= r; k++) 
			{
				if (map.countNeibours(i, j) && (RandomStream.RandRange(1, 100) <= 100 / k))
				{
					map2.set(i, j, true);
				}
			}

		}
	}
	
	map.multiply(map2, 0, 0);
}

TArray<FVector2D> GenerationTools::randomWalk(int seed, FVector2D start, int length)
{
	FVector2D pos = start;
	TArray<FVector2D> res;

	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	for (int i = 0; i < length; i++)
	{
		res.AddUnique(pos);
		pos += {(float)RandomStream.RandRange(-1, 1), (float)RandomStream.RandRange(-1, 1)};
	}
	return res;
}

FVector2D GenerationTools::getCenter(TArray<FVector2D> points)
{
	FVector2D res = {0, 0};

	for (const FVector2D& v : points)
	{
		res += v;
	}
	
	return res / points.Num(); 
}


void GenerationTools::curse(BinaryMap& map, int seed, int chance) 
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	BinaryMap second(map);

	for (int i = 0; i < map.getSize(); i++) 
	{
		for (int j = 0; j < map.getSize(); j++) 
		{
			if (RandomStream.RandRange(1, 100) <= chance) {

				if (map.get(i, j)) 
				{
					if (map.countNeibours(i, j) < 8) second.set(i, j, false);
				}
				else 
				{
					if (map.countNeibours(i, j)) second.set(i, j, true);
				}
			}
		}
	}
	map = second;
}

TArray<TArray<FVector2D>> GenerationTools::genFigures
	(int seed, FVector2D fullSize, int count, bool overlap, int walls, TArray<E_figure> figures, int h, int w, int var)
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	BinaryMap map((std::max)(fullSize.X, fullSize.Y));

	FVector2D size;
	FVector2D pos;
	E_figure figure;

	TArray<TArray<FVector2D>> res;

	for (int k = 0; k < count; k++)
	{
		for (int attemp = 0; attemp <= 30; attemp++)
		{
			pos = {(float)RandomStream.RandRange(0, fullSize.X), (float)RandomStream.RandRange(1, fullSize.Y)};
			size = {(float)RandomStream.RandRange(w - var, w + var), (float)RandomStream.RandRange(h - var, h + var)};
			figure = figures[RandomStream.RandRange(0, figures.Num() - 1)];
		
			if (pos.X + size.X >= fullSize.X || pos.Y + size.Y >= fullSize.Y) continue;

			if (figure == E_figure::rectangle)
			{
				if (!overlap)
				{
					if (map.seekRectangle(pos - walls, size + walls * 2, true))
						continue;
				}
				if (walls) 
				{
					map.multiplyPoints(genRectangle(pos - walls, pos + walls * 2 + size), false);
				}
				res.Add(genRectangle(pos, pos + size));

				map.multiplyPoints(genRectangle(pos, pos + size), true);

				break;
			}
			else if (figure == E_figure::circle)
			{
				if (!overlap)
				{
					if (map.seek(pos, size.X + walls * 2, true))
						continue;
				}
				if (walls)
				{
					map.multiplyPoints(genCircle(pos, size + walls * 2), false);
				}

				res.Add(genCircle(pos, size));
				break;
			}
			else if (figure == E_figure::randomwalk)
			{
				if (!overlap)
				{
					if (map.seekRectangle(pos, size + walls * 2, true))
						continue;
				}
				TArray<FVector2D> walkPoints = randomWalk(seed, pos, h * w);
				res.Add(walkPoints);
				map.multiplyPoints(walkPoints, true);
				break;
			}
		}
	}

	return res;
}

TArray<FVector2D> GenerationTools::genRoads(FVector2D a, FVector2D b, int r)
{
	TArray<FVector2D> res;

	int lenx = b.X - a.X;
	int leny = b.Y - a.Y;

	bool is_x_max = abs(lenx) > abs(leny);

	FVector2D p1;
	if (is_x_max) p1 = {a.X + (b.X - a.X) / 2, a.Y};
	else p1 = {a.X, a.Y + (b.Y - a.Y) / 2};

	FVector2D p2;
	if (is_x_max) p2 = {a.X + (b.X - a.X) / 2, b.Y};
	else p2 = {b.X, a.Y + (b.Y - a.Y) / 2};

	res.Append(genLine(a, p1, r));
	res.Append(genLine(p1, p2, r));
	res.Append(genLine(p2, b, r));

	return res;
}

TArray<GenerationTools::rect> GenerationTools::BSP(rect base, int seed, int minx, int miny)
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	TArray<GenerationTools::rect> rooms = {base};
	TArray<GenerationTools::rect> result = {};
	bool stop = false;

	auto split = [](rect r, bool x_dir, int len) 
	{
		if (x_dir)
		{
			return 
			splitedRect
			{
				rect(r.pos, FVector2D{(float)len, r.size.Y}),
				rect(FVector2D{r.pos.X + len, r.pos.Y}, FVector2D{r.size.X - len, r.size.Y})
			};
		}
		else 
		{
			return 
			splitedRect
			{
				rect(r.pos, FVector2D{r.size.X, (float)len}),
				rect(FVector2D{r.pos.X, r.pos.Y + len}, FVector2D{r.size.X, r.size.Y - len})
			};
		}
	};


	while (!stop)
	{
		stop = true;
		for (const rect& r : rooms)
		{
			if (r.size.X >= minx * 2 || r.size.Y >= miny * 2)
			{
				stop = false;
				
				TArray<char> sidesToSplit;

				if (r.size.X >= minx * 2) sidesToSplit.Add('x');
				if (r.size.Y >= miny * 2) sidesToSplit.Add('y');

				bool is_x_slit = sidesToSplit[RandomStream.RandRange(0, sidesToSplit.Num() - 1)] == 'x';

				auto splitedRooms = split
				(
					r, 
					is_x_slit, 
					RandomStream.RandRange(is_x_slit ? minx : miny, is_x_slit ? (r.size.X - minx) : (r.size.Y - miny))
				);
				result.Add(splitedRooms.first);
				result.Add(splitedRooms.second);
			}
			else 
			{
				result.Add(r);
			}
			
		}
		rooms.Empty();
		rooms = result;
		result.Empty();
	}

	return rooms;
}

TArray<FVector2D> GenerationTools::genLine(FVector2D a, FVector2D b, int r)
{
	TArray<FVector2D> res;

	int lenY = b.Y - a.Y;
	int lenX = b.X - a.X;

	int maxLen = (std::max)(abs(lenY), abs(lenX));
	if (!maxLen) maxLen = 1;

	for (int i = 0; i <= maxLen; i++) 
	{
		for (auto bl : genCircle(FVector2D(a.X + i * lenX / maxLen, a.Y + i * lenY / maxLen), FVector2D(r, r)))
		{
			res.Add(bl);
		}
	}

	return res;
}

TArray<FVector2D> GenerationTools::genSpline(FVector2D a, FVector2D b, bool side)
{
	TArray<FVector2D> res;

	int lenY = b.Y - a.Y;
	int lenX = b.X - a.X;

	FVector2D normal;

	normal.X = a.X + lenX / 2 + lenY / 4 * side - lenY / 4 * !side;
	normal.Y = a.Y + lenY / 2 - lenX / 4 * side - lenY / 4 * !side;

	int maxLen = (std::max)(abs(normal.X - a.X), abs(normal.Y - a.Y)) * 2;
	if (!maxLen) return res;

	for (int j = 0; j <= maxLen; j++) 
	{
		FVector2D a1, b1;

		a1 = FVector2D( a.X + ((normal.X - a.X) * j / maxLen), a.Y + ((normal.Y - a.Y) * j / maxLen) );
		b1 = FVector2D( normal.X + ((b.X - normal.X) * j / maxLen), normal.Y + ((b.Y - normal.Y) * j / maxLen) );

		for (const auto& bl : genCircle 
		(
			FVector2D(a1.X + (b1.X - a1.X) * j / maxLen, a1.Y + (b1.Y - a1.Y) * j / maxLen),
			FVector2D(1, 1)))
		{
			res.Add(bl);
		}
	}

	return res;
}

TArray<FVector2D> GenerationTools::genCircle(FVector2D coord, FVector2D size)
{
	TArray<FVector2D> res;

	for (int i = -size.X; i <= size.X; i++) 
	{
		for (int j = -size.Y; j <= size.Y; j++) 
		{
			if (i * i + j * j <= size.X * size.Y) 
			{
				res.Add({coord.X + i, coord.Y + j});
			}
		}
	}
	return res;
}

TArray<FVector2D> GenerationTools::genRectangle(FVector2D a, FVector2D b)
{
	TArray<FVector2D> res;

	for (int i = (std::min)(a.X, b.X); i < (std::max)(a.X, b.X); i++) 
	{
		for (int j = (std::min)(a.Y, b.Y); j < (std::max)(a.Y, b.Y); j++) 
		{
			res.Add({(float)i, (float)j});
		}
	}
	return res;
}

void GenerationTools::seekConnected(ULayerData* layer, FVector pos, TArray<FVector>& result)
{
	if (result.Num() > 5000) return;
	if (pos.X < 0 || pos.X >= layer->getSize() || pos.Y < 0 || pos.Y >= layer->getSize())
		return;

	FVector connected[] = 
	{
		{(float)pos.X - 1, (float)pos.Y, (float)pos.Z},
		{(float)pos.X, (float)pos.Y - 1, (float)pos.Z},
		{(float)pos.X + 1, (float)pos.Y, (float)pos.Z},
		{(float)pos.X, (float)pos.Y + 1, (float)pos.Z},
		{(float)pos.X, (float)pos.Y, (float)pos.Z + 1},
		{(float)pos.X, (float)pos.Y, (float)pos.Z - 1}
	};
	
	for (const FVector& bl : connected)
	{
		if (layer->getBlock(bl.X, bl.Y, bl.Z == 1) != layer->getBlock(pos.X, pos.Y, pos.Z == 1))
			continue;

		bool flag = false;
		for (const FVector& v : result)
		{
			if (v == bl)
			{
				flag = true;
				break;
			}
		}
		if (flag) continue;

		result.Add(bl);

		seekConnected(layer, bl, result);
	}
}

void GenerationTools::applyCellAutomat(BinaryMap& map, FString born, FString survive)
{
	if (born.Len() != 8 || survive.Len() != 8) return;

    BinaryMap map2(map);

    for (int i = 1; i < map.getSize() - 1; i++)
    {
        for (int j = 1; j < map.getSize() - 1; j++)
        {
            int count = map.countNeibours(i, j);
            if (count == 0)
			{
				map2.set(i, j, false);
				continue;
			}

            if (map.get(i, j))
            { 
                if (survive[8 - count] == '0') 
				{
                    map2.set(i, j, false);
                }
            }
            else
            {
                if (born[8 - count] == '1') 
				{
                    map2.set(i, j, true);
                }
            }
        }
    }
    map = map2;

}

BinaryMap GenerationTools::genNoice(int seed, int size, int chance) 
{
	FRandomStream RandomStream;
	RandomStream.Initialize(seed); 

	BinaryMap map(size);

	for (int i = 0; i < size; i ++) 
	{
		for (int j = 0; j < size; j ++) 
		{
			map.set(i, j, RandomStream.RandRange(1, 100) > chance);
		}
	}

	return map;
}

ULayerData* GenerationTools::genFields(int seed, int size, int count, TArray<FName> fields)
{
	ULayerData* result = NewObject<ULayerData>();
	result->createLayer(size);

	if (count == 0) return result;

	FRandomStream rand;
	rand.Initialize(seed); 

	TMap<FName, TArray<FVector2D>> blocks;

	int freeBlocks = size * 16 * size * 16 - fields.Num();

	for(int i = 0; i < count; i++)
	{
		for (const FName& f : fields)
		{
			while (true)
			{
				FVector2D p = {(float)rand.RandRange(0, size * 16), (float)rand.RandRange(0, size * 16)};

				if (result->getBlock(p.X, p.Y, 0) != "air")
					continue;

				result->setBlock(p.X, p.Y, 0, f);
				
				if (blocks.Find(f))
					blocks[f].Add(p);
				else
					blocks.Add(f, {p});
				break;
			}

		}
	}
	int loopcnt = 0;
	while (freeBlocks >= 0)
	{
		if (loopcnt == size * 16 * size * 16)
		{
			break;
		}

		for(const FName& f : fields)
		{
			int index = rand.RandRange(0, blocks[f].Num() - 1);

			if (!blocks[f].Num()) { 
				continue;
				
			}

			FVector2D bl = blocks[f][index];

			if (result->getBlock(bl.X + 1, bl.Y, 0) == "air" && bl.X + 1 != size * 16)
			{
				result->setBlock(bl.X + 1, bl.Y, 0, f);
				blocks[f].Add(FVector2D{bl.X + 1, bl.Y});
				freeBlocks --;
			}
			if (result->getBlock(bl.X, bl.Y + 1, 0) == "air" && bl.Y + 1 != size * 16)
			{
				result->setBlock(bl.X, bl.Y + 1, 0, f);
				blocks[f].Add(FVector2D{bl.X, bl.Y + 1});
				freeBlocks --;
			}
			if (result->getBlock(bl.X - 1, bl.Y, 0) == "air" && bl.X != 0)
			{
				result->setBlock(bl.X - 1, bl.Y, 0, f);
				blocks[f].Add(FVector2D{bl.X - 1, bl.Y});
				freeBlocks --;
			}
			if (result->getBlock(bl.X, bl.Y - 1, 0) == "air" && bl.Y != 0)
			{
				result->setBlock(bl.X, bl.Y - 1, 0, f);
				blocks[f].Add(FVector2D{bl.X, bl.Y - 1});
				freeBlocks --;
			}

			blocks[f].RemoveAt(index);

		}
		loopcnt ++;
	
	}

	return result;
}

ULayerData* GenerationTools::genVoronoiFields(int seed, int size, int count, bool round, TArray<FName> fields)
{
	ULayerData* result = NewObject<ULayerData>(); result->createLayer(size);

	if (count == 0) return result;

	FRandomStream rand; rand.Initialize(seed); 

	TMap<FName, TArray<FVector>> blocks;

	for(int i = 0; i < count; i++)
	{
		for (const FName& f : fields)
		{
			while (true)
			{
				FVector p = {(float)rand.RandRange(0, size * 16), (float)rand.RandRange(0, size * 16), 0};

				if (result->getBlock(p.X, p.Y, 0) != "air")
					continue;

				result->setBlock(p.X, p.Y, 0, f);
				
				if (blocks.Find(f))
					blocks[f].Add(p);
				else
					blocks.Add(f, {p});
				break;
			}
		}
	}

	int len;
	int len2;

	for(int i = 0; i < size * 16; i++)
	{
		for(int j = 0; j < size * 16; j++)
		{
			len = (size * 16) * (size * 16) + (size * 16) * (size * 16);
			len2 = 0;

			for (const FName& bl : fields)
			{
				for (FVector& v : blocks[bl])
				{
					if (round)
						len2 = (v.X - i) * (v.X - i) + (v.Y - j) * (v.Y - j);
					else
						len2 = abs(v.X - i) + abs(v.Y - j);
					
					
					if (len2 < len)
					{
						len = len2;
						result->setBlock(i, j, 0, bl);
					}
				}
			}
		}
	}

	return result;
}

bool GenerationTools::is_overlap(const TArray<FVector>& first, const TArray<FVector>& second)
{
	for (const auto& f : first)
	{
		for (const auto& s : second)
		{
			if (f == s) return true;
		}
	}

	return false;
}

TArray<FVector2D> GenerationTools::genPoints(int seed, int w, int h, int cnt)
{
	FRandomStream rand;
	rand.Initialize(seed); 

	TArray<FVector2D> result;

	for (int i = 0; i < cnt; i++)
	{
		result.Add({(float)rand.RandRange(0, w), (float)rand.RandRange(0, h)});
	}

	return result;
}
