#!/usr/bin/env python3
"""실측 원본을 검증하고 중앙값과 사분위수를 집계합니다. 표준 라이브러리만 사용합니다."""
from __future__ import annotations
import csv
from collections import defaultdict
from pathlib import Path
from statistics import median

ROOT = Path(__file__).resolve().parents[1]

def main() -> None:
    source = ROOT / 'results' / 'raw_timings.csv'
    groups = defaultdict(list)
    with source.open(newline='', encoding='utf-8') as handle:
        rows = list(csv.DictReader(handle))
    if len(rows) != 630:
        raise ValueError(f'Expected 630 timing observations, got {len(rows)}')
    observed = set()
    seeds = defaultdict(set)
    for row in rows:
        key = (int(row['n']), row['pattern'], row['algorithm'])
        trial = int(row['trial'])
        unique = (*key, trial)
        if row['valid'] != '1' or unique in observed:
            raise ValueError(f'Invalid or duplicate result: {row}')
        observed.add(unique)
        value = float(row['elapsed_ms'])
        if value <= 0:
            raise ValueError('Timer resolution is too low: elapsed_ms must be positive')
        if row['algorithm'] != 'heap' and row['stable_observed'] != '1':
            raise ValueError('Stability violation in a stable algorithm')
        groups[key].append(value)
        seeds[(int(row['n']), row['pattern'], trial)].add(row['seed'])
    if any(len(value) != 1 for value in seeds.values()):
        raise ValueError('Algorithms did not receive matching seeds')
    target = ROOT / 'results' / 'summary.csv'
    with target.open('w', newline='', encoding='utf-8') as handle:
        writer = csv.writer(handle)
        writer.writerow(['n','pattern','algorithm','repeats','median_ms','q1_ms','q3_ms','min_ms','max_ms'])
        for (n, pattern, algorithm), values in sorted(groups.items()):
            if len(values) != 7:
                raise ValueError('Each case requires exactly seven measured runs')
            values.sort()
            # Tukey 방식: 중앙값을 제외한 아래 3개·위 3개의 중앙값.
            q1, q3 = median(values[:3]), median(values[4:])
            writer.writerow([n,pattern,algorithm,7,
                             *[f'{v:.9f}' for v in [median(values),q1,q3,values[0],values[-1]]]])
    print(f'Validated {len(rows)} timed runs; saved {len(groups)} summary rows to {target.name}.')

if __name__ == '__main__':
    main()
