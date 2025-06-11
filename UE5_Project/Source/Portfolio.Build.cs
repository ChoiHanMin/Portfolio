// Copyright Epic Games, Inc. All Rights Reserved.

using System.Diagnostics;
using System.IO;
using UnrealBuildTool;

public class Portfolio : ModuleRules
{
    public Portfolio(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseSharedPCHs;
        //Type = ModuleType.External;
        bUseUnity = false;

        PublicDefinitions.Add("INTERNAL_EXPERIMENTAL=0");

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "NavigationSystem",
            "AIModule",
            "Niagara",
            "EnhancedInput",
            "UMG",
            "Json",
            "JsonUtilities",
            "AndroidRuntimeSettings",
            "AndroidPermission",
            "ChunkDownloader",
            "PakFile",
            "HTTP"

        });

        PrivateIncludePaths.AddRange(
        new string[]
        {
            "Portfolio",
        });

    }

}
