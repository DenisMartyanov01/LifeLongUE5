// Fill out your copyright notice in the Description page of Project Settings.


#include "BinaryMap.h"

BinaryMap::BinaryMap()
{
	size = 16;

	for (int i = 0; i < 16; i++)
	{
		data.Add(TArray<bool>{});

		for (int j = 0; j < 16; j++)
		{			
			data[i].Add(false);
		}
	}
}

BinaryMap::BinaryMap(int size)
{
	this->size = size;

	for (int i = 0; i < size; i++)
	{
		data.Add(TArray<bool>{});

		for (int j = 0; j < size; j++)
		{			
			data[i].Add(false);
		}
	}
}

BinaryMap::BinaryMap(const BinaryMap& second)
{
	data.Empty();

	for (int i = 0; i < second.getSize(); i++)
	{
		data.Add(TArray<bool>{});
		for (int j = 0; j < second.getSize(); j++)
		{
			data[i].Add(second.get(i, j));
		}
	}
	size = second.getSize();
}

BinaryMap::~BinaryMap()
{
	for (int i = 0; i < size; i++)
	{
		data[i].Empty();
	}
	data.Empty();
	size = 0;
}

bool BinaryMap::get(int x, int y) const
{
	if (x < 0 || x >= size || y < 0 || y >= size) return false;
	return data[x][y];
}

void BinaryMap::set(int x, int y, bool val)
{
	if (x < 0 || x >= size || y < 0 || y >= size) return;
	data[x][y] = val;
}

int BinaryMap::countNeibours(int x, int y)
{
	int res = 0;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0) continue;
			if (get(x + i, y + j))
				res++;
		}
	}
	return res;
}

void BinaryMap::invert()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			set(i, j, !get(i, j));
		}
	}
}

void BinaryMap::clear()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			set(i, j, 0);
		}
	}
}

void BinaryMap::expand()
{
	BinaryMap second(this->size * 2);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			second.set(i * 2, j * 2, get(i, j));
			second.set(i * 2 + 1, j * 2, get(i, j));
			second.set(i * 2, j * 2 + 1, get(i, j));
			second.set(i * 2 + 1, j * 2 + 1, get(i, j));
		}
	}

	*this = second;
}

void BinaryMap::thick()
{
	BinaryMap second(*this);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (get(i, j) == false && countNeibours(i, j))
				second.set(i, j, true);
		}
	}
	*this = second;
}

void BinaryMap::smooth()
{
	BinaryMap second(*this);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (get(i, j) == true && countNeibours(i, j) < 8)
				second.set(i,j, false);
		}
	}
	*this = second;
}

int BinaryMap::seek(FVector2D c, int r, bool b)
{
	int result = 0;

	for (int i = -r; i <= r; i++) 
	{
		if (i + c.X >= 0 && i + c.X < getSize()) 
		{
			for (int j = -r; j <= r; j++) 
			{
				if (j + c.Y >= 0 && j + c.Y < getSize()) 
				{
					if (i * i + j * j <= r * r) 
					{
						if (get(c.X + i, c.Y + j) == b)
							result++;
					}
				}
			}
		}
	}

	return result;
}

int BinaryMap::seekRectangle(FVector2D coord, FVector2D new_size, bool b)
{
	int result = 0;

	for (int i = 0; i <= new_size.X; i++) 
	{
		for (int j = 0; j <= new_size.Y; j++) {
			if (get(coord.X + i, coord.Y + j) == b)
				result++;
		}
	}

	return result;
}

void BinaryMap::multiply(const BinaryMap& second, int x, int y)
{
	for (int i = 0; i < second.getSize(); i++) 
	{
		for (int j = 0; j < second.getSize(); j++) 
		{
			if (second.get(i, j))
				set(x + i, y + j, true);
		}
	}
}

void BinaryMap::divide(const BinaryMap& second, int x, int y)
{
	for (int i = 0; i < second.getSize(); i++) 
	{
		for (int j = 0; j < second.getSize(); j++) 
		{
			if (second.get(i, j) == false)
				set(x + i, y + j, false);
		}
	}
}

void BinaryMap::multiplyPoints(const TArray<FVector2D>& points, bool type)
{
	for (int i = 0; i < points.Num(); i++)
	{
		set(points[i].X, points[i].Y, type);
	}
}

void BinaryMap::operator=(const BinaryMap& second)
{
	data.Empty();

	for (int i = 0; i < second.getSize(); i++)
	{
		data.Add(TArray<bool>{});
		for (int j = 0; j < second.getSize(); j++)
		{
			data[i].Add(second.get(i, j));
		}
	}
	size = second.getSize();
}


