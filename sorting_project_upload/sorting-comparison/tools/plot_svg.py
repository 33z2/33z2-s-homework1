#!/usr/bin/env python3
"""실측 CSV를 SVG 그래프로 만듭니다. Python 표준 라이브러리만 사용합니다."""
from __future__ import annotations
import csv
import html
import math
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]

def render() -> None:
    with (ROOT/'results'/'summary.csv').open(newline='', encoding='utf-8') as handle:
        rows = list(csv.DictReader(handle))
    names = ['insertion','merge','heap']
    # 흑백 인쇄용: 색상 대신 선 모양과 점 모양으로 구분합니다.
    dashes = ['', '8 4', '2 4']
    w,h = 880,470
    left,right,top,bottom = 95,850,50,385
    xmin,xmax = math.log10(1000),math.log10(16000)
    ymin,ymax = -2,2
    def x(n): return left+(math.log10(n)-xmin)/(xmax-xmin)*(right-left)
    def y(ms): return bottom-(math.log10(ms)-ymin)/(ymax-ymin)*(bottom-top)
    svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">',
         '<rect width="100%" height="100%" fill="white"/>',
         '<g font-family="sans-serif" font-size="14" fill="black">',
         '<text x="95" y="25" font-size="19">Random input: median sorting time (7 runs)</text>']
    for exp in range(-2,3):
        yy=y(10**exp)
        svg += [f'<path d="M{left} {yy} H{right}" stroke="#dddddd"/>',
                f'<text x="82" y="{yy+5}" text-anchor="end">{10**exp:g}</text>']
    for n in [1000,2000,4000,8000,16000]:
        xx=x(n)
        svg.append(f'<text x="{xx}" y="410" text-anchor="middle">{n:,}</text>')
    svg += [f'<path d="M{left} {top} V{bottom} H{right}" stroke="black" fill="none"/>',
            '<text x="470" y="442" text-anchor="middle">Input size n (log scale)</text>',
            '<text transform="translate(22 220) rotate(-90)" text-anchor="middle">Median time in ms (log scale)</text>']
    for i,name in enumerate(names):
        series=sorted([r for r in rows if r['pattern']=='random' and r['algorithm']==name],key=lambda r:int(r['n']))
        points=' '.join(f'{x(int(r["n"]))},{y(float(r["median_ms"]))}' for r in series)
        svg.append(f'<polyline points="{points}" fill="none" stroke="black" stroke-width="2" stroke-dasharray="{dashes[i]}"/>')
        for r in series:
            xx=x(int(r['n'])); yy=y(float(r['median_ms']))
            y1=y(float(r['q1_ms'])); y3=y(float(r['q3_ms']))
            svg.append(f'<path d="M{xx} {y1} V{y3} M{xx-4} {y1} H{xx+4} M{xx-4} {y3} H{xx+4}" stroke="black"/>')
            svg.append(f'<circle cx="{xx}" cy="{yy}" r="4" fill="white" stroke="black"/>')
        lx=left+i*230
        svg += [f'<path d="M{lx} 462 h40" stroke="black" stroke-width="2" stroke-dasharray="{dashes[i]}"/>',
                f'<text x="{lx+50}" y="467">{html.escape(name)}</text>']
    svg.append('</g></svg>')
    out=ROOT/'report'/'figures'; out.mkdir(parents=True,exist_ok=True)
    (out/'random_time.svg').write_text('\n'.join(svg),encoding='utf-8')
    print('Saved report/figures/random_time.svg (error bars: Q1-Q3).')

if __name__=='__main__': render()
