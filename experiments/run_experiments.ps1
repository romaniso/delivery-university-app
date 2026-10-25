param(
    [Parameter(Mandatory = $true)]
    [string]$Executable
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$executablePath = (Resolve-Path $Executable).Path
$outputDirectory = Join-Path $root "output"
$metricsPath = Join-Path $outputDirectory "metrics.csv"
$experimentsPath = Join-Path $outputDirectory "experiments.csv"

New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null
$results = @()

$scenarios = @(
    @{ Name = "low_load"; File = "experiment-low.txt" },
    @{ Name = "overload"; File = "experiment-overload.txt" },
    @{ Name = "arrival_spike"; File = "experiment-spike.txt" }
)

foreach ($scenario in $scenarios) {
    $inputPath = Join-Path $root (Join-Path "data" $scenario.File)
    $resultPath = Join-Path $outputDirectory ("result-" + $scenario.Name + ".txt")

    & $executablePath $inputPath $resultPath
    if ($LASTEXITCODE -ne 0) {
        throw "Eksperyment '$($scenario.Name)' zakonczyl sie kodem $LASTEXITCODE."
    }
    if (-not (Test-Path $metricsPath)) {
        throw "Program nie utworzyl pliku $metricsPath."
    }

    foreach ($row in (Import-Csv $metricsPath)) {
        $results += [pscustomobject]@{
            scenario = $scenario.Name
            strategy = $row.strategy
            total_deliveries = $row.total_deliveries
            delivered = $row.delivered
            on_time = $row.on_time
            late = $row.late
            remaining = $row.remaining
            average_waiting_days = $row.average_waiting_days
            max_waiting_days = $row.max_waiting_days
            executor_utilization_percent = $row.executor_utilization_percent
        }
    }
}

$results | Export-Csv -Path $experimentsPath -NoTypeInformation -Encoding UTF8
Write-Host "Zapisano wyniki eksperymentow do: $experimentsPath"
