[Setup]
AppName=Carbonator
AppVersion=2.2.0
AppVerName=Carbonator v2.2.0
AppPublisher=Carbinated Audio
DefaultDirName={commonpf64}\Carbinated Audio\Carbonator
DefaultGroupName=Carbinated Audio
OutputBaseFilename=Carbonator-v2.2.0-Windows-Installer
OutputDir=..\..\dist\Windows\Installer
SetupIconFile=..\..\Resources\carbonator-icon.ico
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible
UninstallDisplayName=Carbonator
WizardStyle=modern
DisableProgramGroupPage=yes
LicenseFile=
PrivilegesRequired=admin

[Types]
Name: "full"; Description: "Full installation (VST3 + Standalone)"
Name: "vst3only"; Description: "VST3 plugin only"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "vst3"; Description: "Carbonator VST3 Plugin"; Types: full vst3only custom; Flags: fixed
Name: "standalone"; Description: "Carbonator Standalone App"; Types: full custom

[Files]
Source: "..\..\dist\Windows\VST3\Carbonator.vst3\*"; DestDir: "{commonpf64}\Common Files\VST3\Carbonator.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\dist\Windows\Standalone\Carbonator.exe"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion

[Icons]
Name: "{group}\Carbonator"; Filename: "{app}\Carbonator.exe"; Components: standalone
Name: "{group}\Uninstall Carbonator"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Carbonator"; Filename: "{app}\Carbonator.exe"; Components: standalone; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; Components: standalone; GroupDescription: "Additional options:"

[Run]
Filename: "{app}\Carbonator.exe"; Description: "Launch Carbonator"; Flags: nowait postinstall skipifsilent; Components: standalone

[Messages]
WelcomeLabel2=This will install Carbonator v2.2.0 by Carbinated Audio on your computer.%n%nThe VST3 plugin will be installed to the standard VST3 folder so FL Studio and other DAWs can find it automatically.

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    MsgBox('Carbonator installed successfully!' + #13#10 + #13#10 +
           'VST3 plugin installed to:' + #13#10 +
           ExpandConstant('{commonpf64}') + '\Common Files\VST3\' + #13#10 + #13#10 +
           'Open FL Studio and rescan your plugins to use Carbonator.',
           mbInformation, MB_OK);
  end;
end;
