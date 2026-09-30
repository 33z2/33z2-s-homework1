#!/usr/bin/env python3
"""측정 환경 기록. 실제 실행 시각과 컴파일러 정보를 보관합니다."""
import os
import platform
import subprocess
from datetime import datetime, timezone, timedelta
from pathlib import Path

print('Measured at:', datetime.now(timezone(timedelta(hours=9))).isoformat())
print('OS:', platform.platform())
print('Machine:', platform.machine())
print('CPU count visible:', os.cpu_count())
cpuinfo = Path('/proc/cpuinfo')
if cpuinfo.exists():
    for line in cpuinfo.read_text().splitlines():
        if line.startswith('model name'):
            print('CPU:', line.split(':', 1)[1].strip())
            break
try:
    print('Compiler:', subprocess.check_output(['gcc','--version'], text=True).splitlines()[0])
except (FileNotFoundError, subprocess.CalledProcessError):
    print('Compiler: gcc identification unavailable')
print('Flags: -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror')
print('Timing build: SORT_COUNTING=0; counting build: SORT_COUNTING=1')
print('Timing: CLOCK_MONOTONIC on Linux; QueryPerformanceCounter on Windows')
print('Input: Item { int key; uint32_t original_index; }; size observed on supplied Linux: 8 bytes')
print('Sizes: 1000, 2000, 4000, 8000, 16000; six input patterns; three algorithms')
print('Timing: one warm-up + seven measured runs per algorithm/size/pattern')
print('Counts: first measured seed only, separately executed')
print('Scope: sorting call only; merge buffer malloc/free included; copying/validation excluded')
print('Input seeds: base=20260930, deterministic size/pattern/trial mixing in src/main.c')
print('Execution provenance for bundled results: AI-assisted isolated Linux execution environment; not the student PC')
