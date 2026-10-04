// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class BinaryMap
{
public:
	BinaryMap();
	BinaryMap(int size);
	BinaryMap(const BinaryMap& second);
	~BinaryMap();

	bool get(int x, int y) const;
	void set(int x, int y, bool val);

	int getSize() const { return size; }

	int countNeibours	(int x, int y);

	void invert();
	void clear();
	void expand();

	void thick();
	void smooth();

	int  seek			(FVector2D c, int r, bool b);
	int  seekRectangle	(FVector2D coord, FVector2D size, bool b);

	void multiply		(const BinaryMap& second, int x, int y);
	void divide			(const BinaryMap& second, int x, int y);
	
	void multiplyPoints	(const TArray<FVector2D>& points, bool type);

	void operator =		(const BinaryMap& second);

private:
	
	int size = 16;
	TArray<TArray<bool>> data;
};
