using UnrealBuildTool;

public class LifeLongDreamEditorTarget : TargetRules
{
    public LifeLongDreamEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("Test");
    }
}
