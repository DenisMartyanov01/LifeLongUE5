// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveLoadBase.h"

#include "LayerData.h"
#include "GenerationData.h"

#include "SaveLoad.generated.h"


UCLASS()
class TEST_API USaveLoad : public UBlueprintFunctionLibrary, public SaveLoadBase
{
    GENERATED_BODY()
public:  
    
    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    static void SaveGameLayer               (ULayerData* layer, E_Layer name, FVector2D sector, bool& bSuccess, FString& message);
    static void SaveAsset                   (ULayerData* layer, FString name, bool& bSuccess, FString& message);
    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    static void SaveChunks                  (TArray<UChunkData*> chunks, E_Layer name, bool& bSuccess, FString& message);


    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    static ULayerData* LoadGameLayer        (E_Layer name, FVector2D sector, bool& bSuccess, FString& message);
    static ULayerData* LoadAsset            (FString name, bool& bSuccess, FString& message);
    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    static TArray<UChunkData*> LoadChunks   (TArray<FVector2D> positions, E_Layer name, bool& bSuccess, FString& message);

    
    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    static void SaveArrayToPNG              (const TArray<FColor> Pixels, FString name, int size, bool& bSuccess, FString& message);

    static BiomesData LoadBiomes            ();
    static StructuresData LoadStructures    ();
    static HeightData LoadHeight            (E_Layer layer);

private:
    static void SaveLayer                   (ULayerData* layer, FString path, bool& bSuccess, FString& message);
    static ULayerData* LoadLayer            (FString path, bool& bSuccess, FString& message);
    static void SaveChunksToSector          (TArray<UChunkData*> chunks, E_Layer name, FVector2D sector, bool& bSuccess, FString& message);
    static TArray<UChunkData*> 
        LoadChunksFromSector                (TArray<FVector2D> positions, E_Layer name, FVector2D sector, bool& bSuccess, FString& message);

};