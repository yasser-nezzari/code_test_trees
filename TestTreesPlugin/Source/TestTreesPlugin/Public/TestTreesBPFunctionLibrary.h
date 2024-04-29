// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logging/LogMacros.h"
#include "TestTreesBPFunctionLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TestTreesPluginLog, Log, All);

/**
 * 
 */
UCLASS()
class TESTTREESPLUGIN_API UTestTreesBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "TestTrees Functions")
	static UDataTable* ReadCSVFile(const FString& Path);

	UFUNCTION(BlueprintCallable, Category = "TestTrees Functions")
	static TArray<UDataTable*> KMeanClustering(UDataTable* DataTable, int K, int maxIterations);

};
