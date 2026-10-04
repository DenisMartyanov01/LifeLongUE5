// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoad.h"

#include "ImageUtils.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Modules/ModuleManager.h"

#include "CoordsConvert.h"


void USaveLoad::SaveGameLayer(ULayerData* layer, E_Layer name, FVector2D sector, bool& bSuccess, FString& message)
{
    FString sectorStr = FString::FromInt((int)sector.Y) + "." + FString::FromInt((int)sector.X);
    FString strName = UEnum::GetValueAsString(name);

    SaveLayer(layer, "C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\" + strName + "\\" + sectorStr + ".json", bSuccess, message);

    if (!bSuccess) 
    { 
        UE_LOG(LogTemp, Log, TEXT("%s"), *(message + " form sector " + sector.ToString())); 
    }
}

void USaveLoad::SaveAsset(ULayerData* layer, FString name, bool& bSuccess, FString& message)
{
    SaveLayer(layer, "C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Assets\\structures\\" + name + ".json", bSuccess, message);

}

void USaveLoad::SaveLayer(ULayerData* layer, FString path, bool& bSuccess, FString& message)
{
    if (!layer)
    {
        bSuccess = false;
        message = "Can't read layer";
        return;
    }

    TSharedPtr<FJsonObject> data = MakeShareable(new FJsonObject());

    TArray<TSharedPtr<FJsonValue>> chunks;

    for (int i = 0; i < layer->getChunkCount(); i++) 
    {
        for (int j = 0; j < layer->getChunkCount(); j++) 
        {
            TSharedPtr<FJsonObject> MapJson = MakeShareable(new FJsonObject());

            MapJson->SetStringField("biome", layer->getChunk(i, j)->getBiome());
            MapJson->SetStringField("type", layer->getChunk(i, j)->getType());

            for (const auto& row : *layer->getChunk(i, j)->getData())
            {
                if (row.Value.Num() == 0 || row.Key == "air") continue;

                TArray<TSharedPtr<FJsonValue>> JsonPairArray;

                for (const TPair<int32, int32>& pair : row.Value)
                {
                    TArray<TSharedPtr<FJsonValue>> InnerArray;
                    InnerArray.Add(MakeShared<FJsonValueNumber>(pair.Key));
                    InnerArray.Add(MakeShared<FJsonValueNumber>(pair.Value));

                    JsonPairArray.Add(MakeShared<FJsonValueArray>(InnerArray));
                }

                MapJson->SetArrayField(row.Key.ToString(), JsonPairArray);
            }
            chunks.Add(MakeShareable(new FJsonValueObject(MapJson)));
        }
    }

    data->SetNumberField("chunk_count", layer->getChunkCount());
    data->SetArrayField("chunks", chunks); 

    WriteJsonFile(path, data, bSuccess, message);

}


ULayerData* USaveLoad::LoadGameLayer(E_Layer name, FVector2D sector, bool& bSuccess, FString& message)
{
    FString sectorStr = FString::FromInt((int)sector.Y) + "." + FString::FromInt((int)sector.X);
    FString strName = UEnum::GetValueAsString(name);
    
    ULayerData* result = LoadLayer("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\" + strName + "\\" + sectorStr + ".json", bSuccess, message);

    if (!bSuccess) 
    { 
        UE_LOG(LogTemp, Log, TEXT("%s"), *(message + " form sector " + sector.ToString()));
        return result;
    }

    for (int i = 0; i < result->getChunkCount(); i++)
    {
        for (int j = 0; j < result->getChunkCount(); j++)
        {
            result->getChunk(i, j)->setPosition({result->getChunk(i, j)->getPosition().Y + sector.Y * 64, result->getChunk(i, j)->getPosition().X + sector.X * 64 });
        }
    }

    return result;
}

ULayerData* USaveLoad::LoadAsset(FString name, bool& bSuccess, FString& message)
{
    return LoadLayer("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Assets\\structures\\" + name + ".json", bSuccess, message);
}

ULayerData* USaveLoad::LoadLayer(FString path, bool& bSuccess, FString& message)
{
    TSharedPtr<FJsonObject> data = ReadJsonFile(path, bSuccess, message);

    if (bSuccess == false) 
    {
        message = "Can't read data file";
        return nullptr;
    }

    ULayerData* result = NewObject<ULayerData>();
    result->createLayer(data->GetNumberField(TEXT("chunk_count")));

    TArray<TSharedPtr<FJsonValue>> chunks = data->GetArrayField(TEXT("chunks"));

    for (int i = 0; i < result->getChunkCount(); i++)
    {
        for (int j = 0; j < result->getChunkCount(); j++)
        {
            TMap<FName, TArray<TPair<int, int>>> chunkData;

            auto chunk = chunks[UCoordsConvert::XYToIndex(j, i, 0, result->getChunkCount())];

            for (const auto& element : chunk->AsObject()->Values)
            {
                const FString& Key = element.Key;
                const TSharedPtr<FJsonValue>& Array = element.Value;
                
                if (Array->Type == EJson::Array)
                {
                    chunkData.Add(FName(Key), {});
                    for (const auto& pair : Array->AsArray())
                    {
                        chunkData[FName(Key)].Add(TPair<int, int>{(int)pair->AsArray()[0]->AsNumber(), (int)pair->AsArray()[1]->AsNumber()});
                    }
                }
            }
            result->getChunk(j, i)->setBiome(chunk->AsObject()->GetStringField(TEXT("biome")));
            result->getChunk(j, i)->setType(chunk->AsObject()->GetStringField(TEXT("type")));
            result->getChunk(j, i)->setData(chunkData);
        
            chunkData.Reset();
        }
    }
    return result;
}


void USaveLoad::SaveChunks(TArray<UChunkData*> chunks, E_Layer name, bool& bSuccess, FString& message)
{
    TMap<FVector2D, TArray<UChunkData*>> chunksBySectors;

    for (const auto& chunk : chunks)
    {
        FVector2D sector = { chunk->getPosition().Y / 64, chunk->getPosition().X / 64 };
        if (chunk->getPosition().X < 0) sector.Y -= 1;
        if (chunk->getPosition().Y < 0) sector.X -= 1;

        if (!chunksBySectors.Contains(sector)) 
            chunksBySectors.Add(sector, {});
    
        chunksBySectors[sector].Add(chunk);
    }

    for (const auto& chunksInSector : chunksBySectors)
    {
        SaveChunksToSector(chunksInSector.Value, name, chunksInSector.Key, bSuccess, message);

        if (!bSuccess) UE_LOG(LogTemp, Log, TEXT("%s"), *message);
    }

}

void USaveLoad::SaveChunksToSector(TArray<UChunkData*> chunks, E_Layer name, FVector2D sector, bool& bSuccess, FString& message)
{
    FString sectorStr = FString::FromInt((int)sector.X) + "." + FString::FromInt((int)sector.Y);
    FString strName = UEnum::GetValueAsString(name);

    TSharedPtr<FJsonObject> data = ReadJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\" + strName + "\\" + sectorStr + ".json", bSuccess, message);
    
    if (bSuccess == false) 
    {
        message = "Can't read data file from sector " + sector.ToString();
        return ;
    }

    TArray<TSharedPtr<FJsonValue>> allChunks = data->GetArrayField(TEXT("chunks"));

    for (UChunkData* chunk : chunks)
    {
        TSharedPtr<FJsonObject> MapJson = MakeShareable(new FJsonObject());

        MapJson->SetStringField("biome", chunk->getBiome());
        MapJson->SetStringField("type", chunk->getType());

        for (const auto& row : *chunk->getData())
        {
            TArray<TSharedPtr<FJsonValue>> JsonPairArray;

            for (const TPair<int32, int32>& pair : row.Value)
            {
                TArray<TSharedPtr<FJsonValue>> InnerArray;
                InnerArray.Add(MakeShared<FJsonValueNumber>(pair.Key));
                InnerArray.Add(MakeShared<FJsonValueNumber>(pair.Value));

                JsonPairArray.Add(MakeShared<FJsonValueArray>(InnerArray));
            }

            MapJson->SetArrayField(row.Key.ToString(), JsonPairArray);
        }
        FVector newPos = { (float)((int)chunk->getPosition().X % 64), (float)((int)chunk->getPosition().Y % 64), 0 };
        if (newPos.X < 0) newPos.X += 64;
        if (newPos.Y < 0) newPos.Y += 64;

        allChunks[UCoordsConvert::XYToIndex(newPos.X, newPos.Y, 0, data->GetIntegerField(TEXT("chunk_count")))]
            = MakeShareable(new FJsonValueObject(MapJson));
    }

    WriteJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\" + strName + "\\" + sectorStr + ".json", data, bSuccess, message);
}


TArray<UChunkData*> USaveLoad::LoadChunks(TArray<FVector2D> allPositions, E_Layer name, bool& bSuccess, FString& message)
{
    TArray<UChunkData*> result;

    TMap<FVector2D, TArray<FVector2D>> postionsBySector;

    for (const auto& position : allPositions)
    {
        FVector2D sector = { position.Y / 64, position.X / 64 };
        if (position.X < 0) sector.Y -= 1;
        if (position.Y < 0) sector.X -= 1;
    
        if (!postionsBySector.Contains(sector)) 
            postionsBySector.Add(sector, {});
    
        postionsBySector[sector].Add(position);
    }

    for (const auto& positions : postionsBySector)
    {
        auto loadedChunks = LoadChunksFromSector(positions.Value, name, positions.Key, bSuccess, message);
        if (bSuccess)
            result.Append(loadedChunks);
        else
            UE_LOG(LogTemp, Log, TEXT("%s"), *message);

    }

    return result;
}

TArray<UChunkData*> USaveLoad::LoadChunksFromSector(TArray<FVector2D> positions, E_Layer name, FVector2D sector, bool& bSuccess, FString& message)
{
    FString sectorStr = FString::FromInt((int)sector.X) + "." + FString::FromInt((int)sector.Y);
    FString strName = UEnum::GetValueAsString(name);
    
    TSharedPtr<FJsonObject> data = ReadJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\" + strName + "\\" + sectorStr + ".json", bSuccess, message);

    TArray<UChunkData*> result;

    if (bSuccess == false) 
    {
        message = "Can't read data file from sector " + sector.ToString();
        return result;
    }

    TArray<TSharedPtr<FJsonValue>> chunks = data->GetArrayField(TEXT("chunks"));

    for (const FVector2D& pos : positions)
    {
        TMap<FName, TArray<TPair<int, int>>> chunkData;

        FVector2D newPos = { (float)((int)pos.X % 64), (float)((int)pos.Y % 64) };
        if (newPos.X < 0) newPos.X += 64;
        if (newPos.Y < 0) newPos.Y += 64;

        auto chunk = chunks[UCoordsConvert::XYToIndex(newPos.X, newPos.Y, 0, data->GetNumberField(TEXT("chunk_count")))];

        for (const auto& element : chunk->AsObject()->Values)
        {
            const FString& Key = element.Key;
            const TSharedPtr<FJsonValue>& Array = element.Value;
                
            if (Array->Type == EJson::Array)
            {
                chunkData.Add(FName(Key), {});
                for (const auto& Value : Array->AsArray())
                {
                    chunkData[FName(Key)].Add({Value->AsArray()[0]->AsNumber(), Value->AsArray()[1]->AsNumber()});
                }
            }
        }

        UChunkData* ch = NewObject<UChunkData>();

        ch->setPosition(pos);
        ch->setBiome(chunk->AsObject()->GetStringField(TEXT("biome")));
        ch->setType(chunk->AsObject()->GetStringField(TEXT("type")));
        ch->setData(chunkData);

        result.Emplace(ch);
        
        chunkData.Reset();
    }

    return result;

}


void USaveLoad::SaveArrayToPNG(const TArray<FColor> Pixels, FString name, int size, bool& bSuccess, FString& message)
{
    FString FilePath = "C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Saves\\Photos\\" + name + ".png";

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(Pixels.GetData(), Pixels.GetAllocatedSize(), size, size, ERGBFormat::BGRA, 8))
    {
        auto PNGData = ImageWrapper->GetCompressed();

        FFileHelper::SaveArrayToFile(PNGData, *FilePath);
        UE_LOG(LogTemp, Log, TEXT("PNG saved to %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to encode PNG"));
    }

}


BiomesData USaveLoad::LoadBiomes()
{
    BiomesData result;
    
    FString msg;
    TSharedPtr<FJsonObject> biomesJSON = ReadJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Assets\\biomes.json", result.is_valid, msg );

    if (!result.is_valid)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), *msg); 
        return result;
    }

    for (auto fullBiome : biomesJSON->Values)
    {
        auto biome = fullBiome.Value->AsObject();
        FString method = biome->GetStringField(TEXT("method"));

        Biome b;

        if (method == "perlin")         b.noise = NoiseFabric::getPerlinNoise       (12345, biome->GetNumberField(TEXT("frequency")));
        else if (method == "voronoi")   b.noise = NoiseFabric::getCelluralValueNoise(12345, biome->GetNumberField(TEXT("frequency")));
        else                            b.noise = NoiseFabric::getPerlinNoise       (12345, biome->GetNumberField(TEXT("frequency")));

        if (! biome->HasTypedField(TEXT("blocks"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load blocks from %s biome "), *fullBiome.Key); 
            continue;
        }
        if (! biome->HasTypedField(TEXT("trees"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load trees from %s biome "), *fullBiome.Key); 
            continue;
        }
        if (! biome->HasTypedField(TEXT("ores"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load ores from %s biome "), *fullBiome.Key); 
            continue;
        }
        if (! biome->HasTypedField(TEXT("structures"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load structures from %s biome "), *fullBiome.Key); 
            continue;
        }

        for (auto block : biome->GetArrayField(TEXT("blocks")))
        {
            b.blocks.Add(block->AsString());
        }
        for (auto treeLevel : biome->GetArrayField(TEXT("trees")))
        {
            if (! treeLevel->AsObject()->HasTypedField(TEXT("frequency"), EJson::Number) || 
                ! treeLevel->AsObject()->HasTypedField(TEXT("blocks"), EJson::Array) )
            {
                UE_LOG(LogTemp, Log, TEXT("Can't load tree from %s biome "), *fullBiome.Key); 
                continue;
            }
            TPair<TArray<FString>, float> resultTreeLevel;

            resultTreeLevel.Value = treeLevel->AsObject()->GetNumberField(TEXT("frequency"));
            
            for (auto& treeBlock : treeLevel->AsObject()->GetArrayField(TEXT("blocks")))
                resultTreeLevel.Key.Add(treeBlock->AsString());

            b.trees.Add(resultTreeLevel);
        }
        for (auto ore : biome->GetArrayField(TEXT("ores")))
        {
            b.ores.Add(ore->AsString());
        }
        for (auto structure : biome->GetArrayField(TEXT("structures")))
        {
            b.structures.Add(structure->AsString());
        }

        result.biomes.Add(fullBiome.Key, b);

    }

    result.is_valid = true;
    return result;
}

StructuresData USaveLoad::LoadStructures()
{
    StructuresData result;

    FString msg;
    TSharedPtr<FJsonObject> StructuresJSON = ReadJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Assets\\structures.json", result.is_valid, msg );
    
    if (!result.is_valid)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), *msg); 
        return result;
    }

    for (auto fullStructure : StructuresJSON->Values)
    {
        Structure s;

        s.method = fullStructure.Value->AsObject()->GetStringField(TEXT("method"));

        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("walls"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load walls from %s structure"), *fullStructure.Key); 
            continue;
        }
        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("floor"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load floor from %s structure"), *fullStructure.Key); 
            continue;
        }
        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("objects"), EJson::Array))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load objects from %s structure"), *fullStructure.Key); 
            continue;
        }
        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("room_count"), EJson::Number))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load room_count from %s structure"), *fullStructure.Key); 
            continue;
        }
        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("room_size"), EJson::Number))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load room_size from %s structure"), *fullStructure.Key); 
            continue;
        }
        if (! fullStructure.Value->AsObject()->HasTypedField(TEXT("structure_size"), EJson::Number))
        {
            UE_LOG(LogTemp, Log, TEXT("Can't load structure_size from %s structure"), *fullStructure.Key); 
            continue;
        }

        for (auto wallsBlock : fullStructure.Value->AsObject()->GetArrayField(TEXT("walls")))
        {
            s.walls.Add(wallsBlock->AsString());
        }
        for (auto floorBlock : fullStructure.Value->AsObject()->GetArrayField(TEXT("floor")))
        {
            s.floor.Add(floorBlock->AsString());
        }
        for (auto object : fullStructure.Value->AsObject()->GetArrayField(TEXT("objects")))
        {
            s.objects.Add(object->AsString());
        }

        s.room_count = fullStructure.Value->AsObject()->GetIntegerField     (TEXT("room_count"));
        s.room_size = fullStructure.Value->AsObject()->GetIntegerField      (TEXT("room_size"));
        s.structure_size = fullStructure.Value->AsObject()->GetIntegerField   (TEXT("structure_size"));

        result.structures.Add(fullStructure.Key, s);

    }
    result.is_valid = true;
    return result;
}

HeightData USaveLoad::LoadHeight(E_Layer layer)
{
    HeightData result;

    FString msg;
    TSharedPtr<FJsonObject> StructuresJSON = ReadJsonFile("C:\\Users\\User\\Documents\\Unreal Projects\\Test\\MyFolder\\Assets\\heightMap.json", result.is_valid, msg );

    if (!result.is_valid)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), *msg); 
        return result;
    }

    FString layerStr = UEnum::GetValueAsString(layer);

    if (!StructuresJSON->HasTypedField((TEXT("%s"), layerStr), EJson::Object))
    {
        UE_LOG(LogTemp, Log, TEXT("Can't load heightData for %s layer"), *layerStr); 
        result.is_valid = false;
        return result;
    }
    if (!StructuresJSON->GetObjectField(layerStr)->HasTypedField(TEXT("height"), EJson::Object))
    {
        UE_LOG(LogTemp, Log, TEXT("Can't load height for %s layer"), *layerStr); 
        result.is_valid = false;
        return result;
    }
    if (!StructuresJSON->GetObjectField(layerStr)->HasTypedField(TEXT("biomes"), EJson::Object))
    {
        UE_LOG(LogTemp, Log, TEXT("Can't load biomes for %s layer"), *layerStr); 
        result.is_valid = false;
        return result;
    }

    for (auto heightJson : StructuresJSON->GetObjectField(layerStr)->GetObjectField(TEXT("height"))->Values)
    {
        result.height.Add(TPair<FString, float>{heightJson.Key, heightJson.Value->AsNumber()});
    }
    for (auto biomeJson : StructuresJSON->GetObjectField(layerStr)->GetObjectField(TEXT("biomes"))->Values)
    {
        result.biomes.Add(TPair<FString, float>{biomeJson.Key, biomeJson.Value->AsNumber()});
    }
    
    result.is_valid = true;
    return result;
}
