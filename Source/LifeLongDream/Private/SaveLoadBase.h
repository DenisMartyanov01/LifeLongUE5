// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class TEST_API SaveLoadBase
{
public:

	static FString ReadStringFormFile(FString path, bool& bSuccess, FString& message);
    static void WriteStringToFile(FString path, FString string, bool& bSuccess, FString& message);

    static TSharedPtr<FJsonObject> ReadJsonFile(FString path, bool& bSuccess, FString& message);
    static void WriteJsonFile(FString path, TSharedPtr<FJsonObject> json, bool& bSuccess, FString& message);

};
