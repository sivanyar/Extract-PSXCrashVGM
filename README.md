# Extract-PSXCrashVGM

This is a PowerShell script for extracting music data from the game files of the Crash Bandicoot trilogy.


## Requirements

* PowerShell `>=7.3`
* [CrashEdit](https://github.com/cbhacks/CrashEdit)


## Usage

1.  Copy the `Crash.dll` included with [CrashEdit](https://github.com/cbhacks/CrashEdit) to the directory where this script is located.
2.  Execute this script as follows :

```powershell
Extract-PSXCrashVGM.ps1 -Path E:/S*/*.NSF -Destination ./dst/ -GameVersion 6
```

3. `.SEP`, `.VH`, and `.VB` files are saved in the directory specified by the `-Destination` option.

4. You can convert these files to PSF using tools such as [VGMToolbox](https://sourceforge.net/projects/vgmtoolbox/).  
Generic PSF drivers can play the sounds, but the quality will be very poor. So I recommend the use of the custom driver.  
The `driver` directory contains the PSF driver stub for the Japanese version of Crash Bandicoot 3. If you own a different game, please modify the address references.


### Syntax

```powershell
Extract-PSXCrashVGM.ps1
  -Path <String[]>
  [-Destination <String>]
  [-GameVersion <Int32>]
```

```powershell
Extract-PSXCrashVGM.ps1
  -LiteralPath <String[]>
  [-Destination <String>]
  [-GameVersion <Int32>]
```

### Parameters

Either `-Path` or `-LiteralPath` is mandatory.

`-Path [NSF path]`  
  : Specify the path to the NSF files in the original games. Wildcard characters are permitted.

`-LiteralPath [NSF path]`  
  : Specify the path to the NSF files in the original games. No characters are interpreted as wildcards.

`-Destination [directory path]`  
  : Specify the path where the music data is to be saved. The default is the current directory.

`-GameVersion [version number]`  
  : Specify the version of the original games. the numbers are defined in CrashEdit. The default value is `6`.

```csharp
public enum GameVersion
{
  None,             // 0
  Crash1Beta1995,   // 1
  Crash1BetaMAR08,  // 2
  Crash1BetaMAY11,  // 3
  Crash1,           // 4
  Crash2,           // 5
  Crash3            // 6
}
```


## License

See LICENSE.txt. Note that the license applies to the source codes in this repository. It doesn't apply to any other copyrighted materials, such as external libraries or original game data.
