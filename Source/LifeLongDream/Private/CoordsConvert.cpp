// Fill out your copyright notice in the Description page of Project Settings.


#include "CoordsConvert.h"

int UCoordsConvert::XYToIndex(int x, int y, bool z, int size) 
{
	return z * size * size + y * size + x;
}

FVector UCoordsConvert::IndexToXY(int i, int size)
{
	return FVector{ (double)(i % size), (double)(i / size), (double)(i / (size * size)) };
}


FVector UCoordsConvert::CoordsToBlock(float x, float y, bool z) 
{
	return FVector((int)x / 100, (int)y / 100, z);
}

FVector UCoordsConvert::BlockToCoords(float x, float y, bool z) 
{
	return FVector((int)x * 100, (int)y * 100, z);
}

FVector UCoordsConvert::BlockToChunk(float x, float y, bool z) 
{
	return FVector((int)x / 16, (int)y / 16, z);
}

FVector UCoordsConvert::ChunkToBlock(float x, float y, bool z) 
{
	return FVector((int)x * 16, (int)y * 16, z);
}

FVector UCoordsConvert::CoordsToChunk(float x, float y, bool z) 
{
	return BlockToChunk(CoordsToBlock(x, y, z).X, CoordsToBlock(x, y, z).Y, z);
}

FVector UCoordsConvert::ChunkToCoords(float x, float y, bool z) 
{
	return ChunkToBlock(BlockToCoords(x, y, z).X, BlockToCoords(x, y, z).Y, z);
}
