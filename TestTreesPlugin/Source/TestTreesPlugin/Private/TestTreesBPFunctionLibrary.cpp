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
	// Use plugin path 
	FString File =  FPaths::Combine(PluginDir, "/Binaries/ThirdParty/TestTreesPluginLibrary/Win64/TestTreeData.csv");

	if (!FPaths::FileExists(File))
	{
		UE_LOG(TestTreesPluginLog, Error, TEXT("File %s does not exist in UTestTreesBPFunctionLibrary::ReadCSVFile"), *File);
		return nullptr;
	}

	UClass* DataTableClass = UDataTable::StaticClass(); // get a reference to the type of object 
	UDataTable* DataTable = NewObject<UDataTable>(DataTableClass, FName(TEXT("TreesDataTable"))); // create a new data table object
	DataTable->RowStruct = FTestTreesDataTable::StaticStruct(); // set row structure 
	TArray<FString> CSVLines;
	FFileHelper::LoadANSITextFileToStrings(*File, NULL, CSVLines);

	FTestTreesDataTable Row; 
	int32 ClusterDistance = 0;
	for (int i = 1; i < CSVLines.Num(); i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> stringArray = {};
		aString.ParseIntoArray(stringArray, TEXT(","), false);
		if (stringArray.Num() < 5) { continue; }


		FString LineLabel = stringArray[0];
		if ((LineLabel.Len() == 0) || LineLabel.StartsWith("\";") || LineLabel.StartsWith(";"))
		{
			continue;	// Skip comments or lines with no label
		}

		Row.Height = FCString::Atoi(*stringArray[0]);
		Row.Diameter = FCString::Atoi(*stringArray[1]);

		// Simple CLustering ? 
		if ((i != 1) && (i - 1) % 10 == 0)
		{
			ClusterDistance += 500;
		}

		Row.X = FCString::Atoi(*stringArray[2]); // + ClusterDistance;
		Row.Y = FCString::Atoi(*stringArray[3]); // + ClusterDistance;
		Row.Z = FCString::Atoi(*stringArray[4]);
	
		FName RowName = DataTableUtils::MakeValidName(FString::Printf(TEXT("%d"), i + 1));
		// add the row to the table
		DataTable->AddRow(RowName, Row);
	}
	return DataTable;
}

TArray<UDataTable*> UTestTreesBPFunctionLibrary::KMeanClustering(UDataTable* DataTable, int K, int maxIterations)
{
	if (!DataTable)
	{
		UE_LOG(TestTreesPluginLog, Error, TEXT("DataTable was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
	}

	UClass* DataTableClass = UDataTable::StaticClass();

	// Empty cluster array to return our clusters
	TArray<UDataTable*> Clusters;
	UDataTable* Table = NewObject<UDataTable>(DataTableClass, FName(TEXT("OUtTreesDataTable")));
	Table->RowStruct = FTestTreesDataTable::StaticStruct();
	Clusters.Init(Table, K);

	//Centeroid data will be populated randomly
	UDataTable* CneteroidDataTable = NewObject<UDataTable>(DataTableClass, FName(TEXT("TmpTreesDataTable")));
	CneteroidDataTable->RowStruct = FTestTreesDataTable::StaticStruct();

	if (!CneteroidDataTable)
	{
		UE_LOG(TestTreesPluginLog, Error, TEXT("CneteroidDataTable was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
		return Clusters;
	}

	TArray <FName> RowNames = DataTable->GetRowNames();

	// Init cluster randomly
	for (int32 i = 0; i<K; ++i)
	{
		int32 RandomNbr = FMath::RandRange(0, RowNames.Num() - 2);
		// +2 to avoid empty rows
		const FName RowName = DataTableUtils::MakeValidName(FString::Printf(TEXT("%d"), RandomNbr + 2));
		FTestTreesDataTable* Result = DataTable->FindRow<FTestTreesDataTable>(*RowName.ToString(), *RowName.ToString(), true);
		if (!Result)
		{
			UE_LOG(TestTreesPluginLog, Error, TEXT("Result was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
			return Clusters;
		}
		CneteroidDataTable->AddRow(RowName, *Result);
	}
	
	// The main loop here 
	for (int32 i = 0; i < maxIterations; ++i)
	{
		for (const FName RowName : RowNames)
		{
			float MinDist = FLT_MAX;
			int32 CLosestCenterIdx = 0;
			FTestTreesDataTable* RowData = DataTable->FindRow<FTestTreesDataTable>(*RowName.ToString(), *RowName.ToString(), true);

			if (!RowData)
			{
				UE_LOG(TestTreesPluginLog, Error, TEXT("RowData was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
				return Clusters;
			}

			for (int32 j = 0; j < j; ++i)
			{
				FName CnterRowName = DataTableUtils::MakeValidName(FString::Printf(TEXT("%d"), j + 1));
				FTestTreesDataTable* CenterRowData = CneteroidDataTable->FindRow<FTestTreesDataTable>(*CnterRowName.ToString(), *CnterRowName.ToString(), true);

				if (!CenterRowData)
				{
					UE_LOG(TestTreesPluginLog, Error, TEXT("CenterRowData was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
					return Clusters;
				}

				/*FVector CentroidVec(CenterRowData->X, CenterRowData->Y, CenterRowData->Z);
				FVector InVec(RowData->X, RowData->Y, RowData->Z);*/

				// Cange the logig here the classification
				float Distance = FMath::Square(CenterRowData->X - RowData->X) + FMath::Square(CenterRowData->Y - RowData->Y) + 
					FMath::Square(CenterRowData->Z - RowData->Z) + FMath::Square(CenterRowData->Diameter - RowData->Diameter) + 
					FMath::Square(CenterRowData->Height - RowData->Height);

				//float Distance = FMath::Square(CenterRowData->Height - RowData->Height);

				Distance = FMath::Sqrt(Distance);

				if (Distance < MinDist)
				{
					MinDist = Distance;
					CLosestCenterIdx = j;
				}
			}
			Clusters[CLosestCenterIdx]->AddRow(RowName, *RowData);
		}
		// Update the cluster centroid
		for (int32 j = 0; j < j; ++i)
		{
			if (Clusters.Num() > 0)
			{
				FVector SumPos = FVector::Zero();
				TArray <FName> ClusterRowNames = Clusters[j]->GetRowNames();
				for (FName& ClusterName : ClusterRowNames)
				{
					FTestTreesDataTable* ClusterRowData = Clusters[j]->FindRow<FTestTreesDataTable>(*ClusterName.ToString(), *ClusterName.ToString(), true);

					if (!ClusterRowData)
					{
						UE_LOG(TestTreesPluginLog, Error, TEXT("ClusterRowData was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
						return Clusters;
					}

					FVector ClusterVec(ClusterRowData->X, ClusterRowData->Y, ClusterRowData->Z);
					SumPos += ClusterVec;
				}

				FVector PosCenter = SumPos / ClusterRowNames.Num();

				FName CnterRowName = DataTableUtils::MakeValidName(FString::Printf(TEXT("%d"), j + 1));
				FTestTreesDataTable* CenterRowData = CneteroidDataTable->FindRow<FTestTreesDataTable>(*CnterRowName.ToString(), *CnterRowName.ToString(), true);

				if (!CenterRowData)
				{
					UE_LOG(TestTreesPluginLog, Error, TEXT("CenterRowData was nullptr in UTestTreesBPFunctionLibrary::KMeanClustering"));
					return Clusters;
				}

				CenterRowData->X = PosCenter.X;
				CenterRowData->Y = PosCenter.Y;
				CenterRowData->Z = PosCenter.Z;

			}
		}

	}

	return Clusters;
}
