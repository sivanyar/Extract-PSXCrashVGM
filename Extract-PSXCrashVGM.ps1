#!/usr/bin/env pwsh

[CmdletBinding(DefaultParameterSetName = 'Path')]
Param (
  [SupportsWildCards()]
  [Parameter(
    Position = 0,
    ParameterSetName = 'Path',
    ValueFromPipeline = $false,
    ValueFromPipelineByPropertyName = $true
  )]
  [String[]]$Path,

  [Alias('LP')]
	[Alias('PSPath')]
  [Parameter(
    Position = 0,
    ParameterSetName = 'LiteralPath',
    ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true
  )]
  [String[]]$LiteralPath,

  [String]$Destination = $PWD,
  [Int32]$GameVersion = 6   # [Crash.GameVersion]::Crash3
)

Begin {
  Add-Type -LiteralPath $PSScriptRoot/Crash.dll

  function FindLinkedVB {
    $samples = [Collections.Generic.List[Crash.SampleLine]]::new()
    # Calling the generic method (PowerShell 7.3 or higher is required)
    $vb0entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB0EID)
    $vb1entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB1EID)
    $vb2entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB2EID)
    $vb3entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB3EID)
    $vb4entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB4EID)
    $vb5entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB5EID)
    $vb6entry = $NSF.GetEntry[Crash.WavebankEntry]($MusicEntry.VB6EID)
    if ($null -ne $vb0entry) {
      $samples.AddRange($vb0entry.Samples.SampleLines);
    }
    if ($null -ne $vb1entry) {
      $samples.AddRange($vb1entry.Samples.SampleLines);
    }
    if ($null -ne $vb2entry) {
      $samples.AddRange($vb2entry.Samples.SampleLines);
    }
    if ($null -ne $vb3entry) {
      $samples.AddRange($vb3entry.Samples.SampleLines);
    }
    if ($null -ne $vb4entry) {
      $samples.AddRange($vb4entry.Samples.SampleLines);
    }
    if ($null -ne $vb5entry) {
      $samples.AddRange($vb5entry.Samples.SampleLines);
    }
    if ($null -ne $vb6entry) {
      $samples.AddRange($vb6entry.Samples.SampleLines);
    }
    return ,$samples;
  }
}

Process {
  trap {
    break
  }

  switch ($PSCmdlet.ParameterSetName) {
    'Path' {
      $Data = @(Get-Content -Path $Path -Raw -AsByteStream)
      break
    }
    'LiteralPath' {
      $Data = @(Get-Content -LiteralPath $LiteralPath -Raw -AsByteStream)
      break
    }
    Default {
      Write-Error -Message "Invalid parameter."
      return
    }
  }

  for ($i = 0; $i -lt $Data.Count; $i++) {
    $NSF = [Crash.NSF]::LoadAndProcess($Data[$i], $GameVersion)

    $MusicEntries = @($NSF.GetEntries[Crash.MusicEntry]())
    $BaseName = $Data[$i].PSChildName.Split(".")[0]

    foreach ($MusicEntry in $MusicEntries) {
      $MusicIndex = $MusicEntry.EName[2]
      if ($null -ne $MusicEntry.SEP) {
        $OutFilePath = Join-Path $Destination "$BaseName($MusicIndex).SEP"
        Set-Content -Value $MusicEntry.SEP.Save() -LiteralPath $OutFilePath -AsByteStream
      }

      $VH = $NSF.GetEntry[Crash.MusicEntry]($MusicEntry.VHEID).VH
      if ($null -ne $VH) {
        $OutFilePath = Join-Path $Destination "$BaseName($MusicIndex).VH"
        Set-Content -Value $VH.Save() -LiteralPath $OutFilePath -AsByteStream
      }

      $VB = FindLinkedVB
      if ($null -ne $VB) {
        $OutFilePath = Join-Path $Destination "$BaseName($MusicIndex).VB"
        Set-Content -Value ([Crash.SampleSet]::new($VB).Save()) -LiteralPath $OutFilePath -AsByteStream
      }
    }
  }
}
