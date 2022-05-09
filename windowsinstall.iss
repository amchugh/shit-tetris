[Setup]
AppVersion=1.0
AppName=Shit Tetris
DefaultDirName={autopf}/shittetris

[Dirs]
Name: "{app}/rsc"

[Files]
Source: "./build/Debug/shittetris.exe"; DestDir: "{app}"
Source: "./rsc/*"; DestDir: "{app}/rsc"

; VC++ redistributable runtime. Extracted by VC2015RedistNeedsInstall(), if needed.
Source: "./windows/VC_redist.x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall

[Icons]
Name: "{commondesktop}\Shit Tetris"; Filename: "{app}/shittetris.exe"; WorkingDir: "{app}"

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; StatusMsg: "Installing VC2015 redist}"; \
  Parameters: "/quiet"; Check: VC2015RedistNeedsInstall ; Flags: waituntilterminated

[Code]
function VC2015RedistNeedsInstall: Boolean;
var 
  Version: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
       'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version', Version) then
  begin
    // Is the installed version at least 14.14 ? 
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.14.26429.03')<0);
  end
  else 
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('VC_redist.x64.exe');
  end;
end;
