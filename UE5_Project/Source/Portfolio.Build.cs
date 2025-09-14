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
       // string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
       // PublicIncludePaths.Add(Path.Combine(PluginPath, "ThirdParty/firebase/include/firebase"));
       // PrivateIncludePaths.Add(Path.Combine(PluginPath, "ThirdParty/firebase/include/firebase"));

        /*
         *   "E:\Unreal\Portfolio\Source\Portfolio\"
         */

        PublicDefinitions.Add("INTERNAL_EXPERIMENTAL=0");

       //if (Target.Platform == UnrealTargetPlatform.Android)
       //{
       //    AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "ExtraUPL.xml"));
       //    string FirebaseLibPath = Path.Combine(ModuleDirectory, "ThirdParty/firebase/libs/android/arm64-v8a");
       //    PublicAdditionalLibraries.Add(Path.Combine(FirebaseLibPath, "libfirebase_app.a"));
       //    PublicAdditionalLibraries.Add(Path.Combine(FirebaseLibPath, "libfirebase_functions.a"));
       //    PublicAdditionalLibraries.Add(Path.Combine(FirebaseLibPath, "libfirebase_firestore.a"));
       //    PublicAdditionalLibraries.Add(Path.Combine(FirebaseLibPath, "libfirebase_storage.a"));
       //    PublicAdditionalLibraries.Add(Path.Combine(FirebaseLibPath, "libfirebase_auth.a"));
       //}
       // else if (Target.Platform == UnrealTargetPlatform.Win64)
       // {
       //    
       //     PublicSystemLibraries.Add("ws2_32.lib");
       //     PublicSystemLibraries.Add("crypt32.lib");
       //
       //     string LibPath = Path.Combine(ModuleDirectory, "ThirdParty/firebase/libs/windows/VS2019/MD/x64/Debug");
       //     PublicAdditionalLibraries.Add(Path.Combine(LibPath, "firebase_app.lib"));
       //     PublicAdditionalLibraries.Add(Path.Combine(LibPath, "firebase_functions.lib"));
       //     PublicAdditionalLibraries.Add(Path.Combine(LibPath, "firebase_firestore.lib"));
       //     PublicAdditionalLibraries.Add(Path.Combine(LibPath, "firebase_storage.lib"));
       //     PublicAdditionalLibraries.Add(Path.Combine(LibPath, "firebase_auth.lib"));
       //
       // }
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
            "PhysicsCore",
            "HTTP",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"

        });

        PrivateIncludePaths.AddRange(
        new string[]
        {
            "Portfolio",
        });

    }

}
