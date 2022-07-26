// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class skating_fpsServerTarget : TargetRules //Change this line according to the name of your project
{
    public skating_fpsServerTarget(TargetInfo Target) : base(Target) //Change this line according to the name of your project
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("skating_fps"); //Change this line according to the name of your project
    }
}