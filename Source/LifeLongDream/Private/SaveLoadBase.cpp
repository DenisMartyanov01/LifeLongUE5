// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveLoadBase.h"

#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
//#include "Factories/ReimportDataTableFactory.h"
#include "Modules/ModuleManager.h"

#include "CoordsConvert.h"


FString SaveLoadBase::ReadStringFormFile(FString path, bool& bSuccess, FString& message) 
{

    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
    {
        bSuccess = 0;
        message = FString::Printf(TEXT("File doesn't exist %s"), *path);
        return "";
    }

    FString string = "";

    if (!FFileHelper::LoadFileToString(string, *path))
    {
        bSuccess = 0;
        message = FString::Printf(TEXT("Broken file %s"), *path);
        return "";
    }

    message = FString::Printf(TEXT("File successfully rode %s"), *path);
    bSuccess = 1;
    return string;
}

void SaveLoadBase::WriteStringToFile(FString path, FString string, bool& bSuccess, FString& message)
{

    if (!FFileHelper::SaveStringToFile(string, *path))
    {
        bSuccess = 0;
        message = FString::Printf(TEXT("Broken file %s"), *path);
        return;
    }

    message = FString::Printf(TEXT("File successfully wrote %s"), *path);
    bSuccess = 1;
}

TSharedPtr<FJsonObject> SaveLoadBase::ReadJsonFile(FString path, bool& bSuccess, FString& message)
{
    FString JsonString = ReadStringFormFile(path, bSuccess, message);
  
    if (!bSuccess)
    {
        return nullptr;
    }

    TSharedPtr<FJsonObject> JsonObject;

    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonString), JsonObject))
    {
        bSuccess = 0;
        message = FString::Printf(TEXT("Broken file %s"), *path);
        return nullptr;
    }

  
    message = FString::Printf(TEXT("File successfully rode %s"), *path);
    bSuccess = 1;
    return JsonObject;

}


void SaveLoadBase::WriteJsonFile(FString path, TSharedPtr<FJsonObject> json, bool& bSuccess, FString& message)
{
    FString JsonString;

    if (!FJsonSerializer::Serialize(json.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
    {
        bSuccess = 0;
        message = FString::Printf(TEXT("Broken object"));
        return;
    }

    WriteStringToFile(path, JsonString, bSuccess, message);
    if (!bSuccess)
    {
        return;
    }

    bSuccess = true;
    message = FString::Printf(TEXT("File rode successfully %s"), *path);

}
