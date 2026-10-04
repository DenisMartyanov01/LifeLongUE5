using UnrealBuildTool;

public class LifeLongDreamTarget : TargetRules
{
    public LifeLongDreamTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("Test");
    }
}
