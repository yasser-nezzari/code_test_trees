// Fill out your copyright notice in the Description page of Project Settings.


#include "TestTreesBPFunctionLibrary.h"
#include "TestTreesDataTable.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

DEFINE_LOG_CATEGORY(TestTreesPluginLog);

UDataTable* UTestTreesBPFunctionLibrary::ReadCSVFile(const FString& FileName)
{

	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("TestTreesPlugin");

	if (!Plugin.IsValid())
	{
		UE_LOG(TestTreesPluginLog, Error, TEXT("Invalid Plugin in UTestTreesBPFunctionLibrary::ReadCSVFile"));
		return nullptr;
	}
	const FString PluginDir = Plugin->GetBaseDir();
	
	//FString File =  FPaths::Combine(PluginDir, "/Binaries/ThirdParty/TestTreesPluginLibrary/Win64/TestTreeData.csv");

	FString File = FPaths::Combine(PluginDir, "/Content/db/TestTreeData.csv");

	if (!FPaths::FileExists(File))
	{
		UE_LOG(TestTreesPluginLog, Error, TEXT("File %s does not exist in UTestTreesBPFunctionLibrary::ReadCSVFile"), *File);
		return nullptr;
	}

	UClass* DataTableClass = UDataTable::StaticClass(); // get a reference to the type of object we are going to use, in this case the basic DataTable, but you can have your own
	UDataTable* DataTable = NewObject<UDataTable>(DataTableClass, FName(TEXT("TreesDataTable"))); // create a new data table object
	DataTable->RowStruct = FTestTreesDataTable::StaticStruct(); // set what kind of row structure we are going to use for our table, we assume that you included the example row structure here


	//UDataTable* DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, *File));
	TArray<FString> CSVLines;
	FFileHelper::LoadANSITextFileToStrings(*File, NULL, CSVLines);

	FTestTreesDataTable Row; // set the type of row we are going to use
	// iterate the csv lines and populate the row with the values from the csv

	int32 ClusterDistance = 0;
	for (int i = 1; i < CSVLines.Num(); i++)
	{
		FString aString = CSVLines[i];

		UE_LOG(TestTreesPluginLog, Error, TEXT("aString = %s"), *aString);
	
		TArray<FString> stringArray = {};
		aString.ParseIntoArray(stringArray, TEXT(","), false);
		if (stringArray.Num() == 0) { continue; }


		FString LineLabel = stringArray[0];
		if ((LineLabel.Len() == 0) || LineLabel.StartsWith("\";") || LineLabel.StartsWith(";"))
		{
			continue;	// Skip comments or lines with no label
		}

		Row.Height = FCString::Atoi(*stringArray[0]);
		Row.Diameter = FCString::Atoi(*stringArray[1]);
		Row.X = FCString::Atoi(*stringArray[2]);

		if ((i != 1) && (i - 1) % 10 == 0)
		{
			ClusterDistance += 5000;
		}

		Row.Y = FCString::Atoi(*stringArray[3]) + ClusterDistance;;
		Row.Z = FCString::Atoi(*stringArray[4]);
	
		FName RowName = DataTableUtils::MakeValidName(FString::Printf(TEXT("%d"), i + 1));
		// add the row to the table
		DataTable->AddRow(RowName, Row);
	}

	return DataTable;
}
