#!/usr/bin/env bash
# Odpowiednik run_experiments.ps1 dla macOS/Linux (nie wymaga PowerShella).
# Uzycie: ./experiments/run_experiments.sh <sciezka_do_programu> [A|B|C]

set -euo pipefail

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Uzycie: $0 <sciezka_do_programu> [A|B|C]" >&2
    exit 1
fi
strategyArgs=()
if [ "$#" -eq 2 ]; then
    strategyArgs=(--strategy "$2")
fi
if [ ! -x "$1" ]; then
    echo "Nie znaleziono programu: $1" >&2
    exit 1
fi

root="$(cd "$(dirname "$0")/.." && pwd)"
executable="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
outputDirectory="$root/output"
metricsPath="$outputDirectory/metrics.csv"
experimentsPath="$outputDirectory/experiments.csv"

mkdir -p "$outputDirectory"
echo "scenario,strategy,total_deliveries,delivered,on_time,late,remaining,average_waiting_days,max_waiting_days,executor_utilization_percent" > "$experimentsPath"

scenarios=(
    "low_load:experiment-low.txt"
    "overload:experiment-overload.txt"
    "arrival_spike:experiment-spike.txt"
)

for scenario in "${scenarios[@]}"; do
    name="${scenario%%:*}"
    file="${scenario#*:}"
    rm -f "$metricsPath"

    if ! "$executable" ${strategyArgs[@]+"${strategyArgs[@]}"} "$root/data/$file" "$outputDirectory/result-$name.txt"; then
        echo "Eksperyment '$name' zakonczyl sie bledem." >&2
        exit 1
    fi
    if [ ! -f "$metricsPath" ]; then
        echo "Program nie utworzyl pliku $metricsPath." >&2
        exit 1
    fi

    tail -n +2 "$metricsPath" | sed "s/^/$name,/" >> "$experimentsPath"
done

echo "Zapisano wyniki eksperymentow do: $experimentsPath"
