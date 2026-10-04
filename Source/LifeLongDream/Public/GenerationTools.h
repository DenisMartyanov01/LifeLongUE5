// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LayerData.h"
#include "BinaryMap.h"

class TEST_API GenerationTools
{
public:
	
	enum   E_figure		{ circle, rectangle, randomwalk };
	struct rect			{ FVector2D pos; FVector2D size; };
	struct splitedRect	{ rect first; rect second; };

	static TArray<FVector2D> genPoints	(int seed, int w, int h, int cnt);
	static TArray<TArray<FVector2D>> 
		genFigures						(int seed, FVector2D size, int count, bool overlap, int walls, TArray<E_figure> figures, int h, int w, int var);
	static TArray<FVector2D> randomWalk	(int seed, FVector2D start, int length);
	static FVector2D getCenter			(TArray<FVector2D> points);
	static TArray<FVector2D> genRoads	(FVector2D a, FVector2D b, int r);
	static TArray<rect> BSP				(rect base, int seed, int minx, int miny); 

	static TArray<FVector2D> genLine	(FVector2D a, FVector2D b, int r);
	static TArray<FVector2D> genSpline	(FVector2D a, FVector2D b, bool side);
	static TArray<FVector2D> genCircle	(FVector2D coord, FVector2D size);
	static TArray<FVector2D> genRectangle(FVector2D a, FVector2D b);

	static void curse					(BinaryMap& map, int seed, int chance);
	static void seekConnected			(ULayerData* layer, FVector pos, TArray<FVector>& result);
	static void spread					(BinaryMap& map, int seed, int r);
	static void applyCellAutomat		(BinaryMap& map, FString born, FString survive);

	static BinaryMap genNoice			(int seed, int size, int chance);
	static ULayerData* genFields		(int seed, int size, int count, TArray<FName> fields);
	static ULayerData* genVoronoiFields	(int seed, int size, int count, bool round, TArray<FName> fields);

	static bool is_overlap				(const TArray<FVector>& first, const TArray<FVector>& second);

};
