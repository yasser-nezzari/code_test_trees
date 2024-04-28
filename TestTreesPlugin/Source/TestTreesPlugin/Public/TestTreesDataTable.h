// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TestTreesDataTable.generated.h"

/**
 * 
 */
USTRUCT()
struct TESTTREESPLUGIN_API FTestTreesDataTable : public FTableRowBase
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AtkinsTrees")
	float Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AtkinsTrees")
	float Diameter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AtkinsTrees")
	float X;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AtkinsTrees")
	float Y;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AtkinsTrees")
	float Z;

};
