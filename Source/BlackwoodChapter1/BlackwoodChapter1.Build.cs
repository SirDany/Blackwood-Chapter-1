// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlackwoodChapter1 : ModuleRules
{
	public BlackwoodChapter1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"BlackwoodChapter1",
			"BlackwoodChapter1/Variant_Horror",
			"BlackwoodChapter1/Variant_Horror/UI",
			"BlackwoodChapter1/Variant_Shooter",
			"BlackwoodChapter1/Variant_Shooter/AI",
			"BlackwoodChapter1/Variant_Shooter/UI",
			"BlackwoodChapter1/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
