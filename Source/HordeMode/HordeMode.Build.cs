// Copyright (c) 2020 Russ 'trdwll' Treadwell

using UnrealBuildTool;

public class HordeMode : ModuleRules
{
	public HordeMode(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem" });
	}
}
