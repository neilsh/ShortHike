; --------------------------------------------------------------------------------
; ShortHike Unstable setup script

#define BUILD_UNSTABLE

[Setup]
;--------------------------------------------------------------------------------
; Generic information about application

#ifdef BUILD_UNSTABLE
 AppName=ShortHike [Unstable]
 AppVerName=ShortHike 2 [Unstable]
 OutputBaseFilename=ShortHikeSetup_Unstable
 DefaultDirName={pf}\ShortHike_Unstable
 DefaultGroupName=ShortHike [Unstable]
 Compression=lzma/fast
#else
 AppName=ShortHike
 AppVerName=ShortHike 2
 OutputBaseFilename=ShortHikeSetup
 DefaultDirName={pf}\ShortHike
 DefaultGroupName=ShortHike

 ; Use lzma/fast lzma/ultra or lzma/max
 Compression=lzma/fast
#endif
UninstallDisplayIcon={app}\assets\ShortHike.ico

AppPublisher=Mistaril
AppPublisherURL=http://www.ShortHike.com
AppSupportURL=http://www.ShortHike.com/support/
AppComments="ShortHike space station game"
LicenseFile=setup\license.txt

DisableReadyPage=yes
DisableStartupPrompt=yes

SolidCompression=yes

WizardImageFile=setup\icons\InstallerImage.bmp
WizardSmallImageFile=setup\icons\icon_55.bmp

SourceDir=..

[Files]
Source: target_binary/binary_win32/*; DestDir: "{app}/binary_win32"; Flags: overwritereadonly ignoreversion
Source: "{app}\binary_win32\*.exe"; DestDir: "{app}"; Flags: external overwritereadonly ignoreversion
Source: "{app}\binary_win32\*.dll"; DestDir: "{app}"; Flags: external overwritereadonly ignoreversion
Source: target_assets/*; DestDir: "{app}"; Flags: recursesubdirs overwritereadonly ignoreversion
Source: target_scm/*; DestDir: "{app}"; Flags: recursesubdirs overwritereadonly ignoreversion
Source: setup/license.txt; DestDir: "{app}"; Flags: overwritereadonly ignoreversion
Source: setup/*.url; DestDir: "{group}"; Flags: ignoreversion

[UninstallDelete]
Type: filesandordirs; Name: {app}\assets
Type: filesandordirs; Name: {app}\scm
Type: filesandordirs; Name: {app}\binary_win32
Type: filesandordirs; Name: {app}\*.log
Name: {app}\*.dll; Type: files
Name: {app}\*.exe; Type: files
Name: {app}\*.txt; Type: files

#ifdef BUILD_UNSTABLE
 [Icons]
 Name: "{group}\ShortHike [Unstable]"; Filename: "{app}\ShortHike.exe"; WorkingDir: "{app}"; IconFilename: "{app}\assets\ShortHike.ico"
 Name: "{userdesktop}\ShortHike [Unstable]"; Filename: "{app}\ShortHike.exe"; WorkingDir: "{app}"; IconFilename: "{app}\assets\ShortHike.ico";
#else
 [Icons]
 Name: "{group}\ShortHike"; Filename: "{app}\ShortHike.exe"; WorkingDir: "{app}"; IconFilename: "{app}\assets\ShortHike.ico"
 Name: "{userdesktop}\ShortHike"; Filename: "{app}\ShortHike.exe"; WorkingDir: "{app}"; IconFilename: "{app}\assets\ShortHike.ico";
#endif

[Run]
Filename: "{app}\ShortHike.exe"; WorkingDir: "{app}"; Description: "Launch ShortHike"; Flags: nowait postinstall
