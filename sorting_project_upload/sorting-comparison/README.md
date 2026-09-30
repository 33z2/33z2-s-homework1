# 정렬 비교 과제: 삽입 · 병합 · 힙

학번 2026193114 · 이름 이현재

수업에서 배운 삽입·병합 정렬과, 제시된 학습 목록에 없는 힙 정렬을 C17로 구현하여 비교합니다.
보고서 3~4쪽에는 AI를 활용하여 정리한 힙 정렬 학습 문답이 있습니다.

## 제출 전에 반드시 확인

**이 프로젝트의 ZIP은 GitHub에 올릴 파일을 모은 업로드용 압축본입니다. 과제에서 요구한 GitHub의 `Code > Download ZIP` 산출물은 아닙니다.**

1. 본인 GitHub 저장소에 압축을 푼 파일들을 올립니다. ZIP 파일 하나만 올리지 마세요.
2. `report/REPORT.pdf` 첫 페이지의 `GITHUB_REPOSITORY_URL`을 실제 본인 저장소 주소로 바꾸고 저장합니다. PDF 입력이 어려우면 `REPORT.docx`를 수정한 뒤 PDF로 내보냅니다. `REPORT.md`도 같은 주소로 수정합니다.
3. 최종 내용을 커밋한 본인 저장소에서 `Code > Download ZIP`을 선택합니다. 제출물은 URL을 기재한 PDF와 GitHub에서 내려받은 ZIP입니다.

자세한 단계는 `SUBMISSION_GUIDE.md`를 확인하세요. 저장소 생성·업로드·학교 제출은 아직 이루어지지 않았습니다.

## 준비와 실행

교수자 제공 템플릿: https://github.com/lec-algorithm/algorithm-env

이 템플릿으로 본인 저장소를 만든 경우, 기존 예제 `src/`, `tests/`, `Makefile`을 이 프로젝트의 것으로 교체하고 템플릿의 `.devcontainer/`, `Dockerfile`, `compose.yml`은 유지합니다. 여기에는 검증하지 않은 대체 컨테이너 설정을 넣지 않았습니다.

GCC, make, Python 3가 있는 Linux 터미널에서 프로젝트 최상위 폴더로 이동하여 실행합니다.

```sh
make run       # 정렬 결과와 안정성 반례 출력
make test      # 카운터를 켠 빌드/끈 빌드를 각각 검사
make bench     # 630회 시간 측정, 90개 연산 횟수 측정 및 집계
make sanitize  # AddressSanitizer + UndefinedBehaviorSanitizer 검사
```

C 정렬·테스트는 외부 라이브러리를 사용하지 않습니다. CSV 집계와 SVG 생성 Python 코드도 표준 라이브러리만 사용합니다.
직접 검증한 환경은 Linux x86_64 / GCC 14.2.0 / C17입니다. Windows 타이머 분기는 코드에 있지만 Windows에서 실행 검증한 것은 아닙니다.
여러 C 파일을 함께 링크해야 하므로 `src/main.c` 하나만 따로 컴파일하지 말고 `make`를 사용하세요.

## 파일 구성

| 위치 | 내용 |
| --- | --- |
| `src/insertionSort.c` | 삽입 정렬 |
| `src/mergeSort.c` | 재귀 병합 정렬 |
| `src/heapSort.c` | 반복문 기반 최대 힙 정렬 |
| `src/sort.h`, `src/sort.c` | 공통 자료형·연산 카운터·알고리즘 목록 |
| `src/data.*`, `src/main.c` | 입력 생성·측정·검증·실행 |
| `tests/test_sort.c` | 기본·전수·난수·경계·안정성 테스트 |
| `tools/` | CSV 집계, 환경 기록, SVG 시간 그래프 |
| `results/` | 측정 원본·집계표·검증 로그 |
| `report/` | 8쪽 보고서(PDF, 수정용 DOCX, Markdown), 그래프 |

## 실험 조건과 지표

크기 1,000~16,000의 다섯 종류, 입력 형태 여섯 종류, 정렬 세 종류를 비교했습니다. 각 조건은 워밍업 1회 후 7회 측정합니다.
여섯 입력은 무작위·정렬됨·역순·거의 정렬됨(인접 교환)·16종 중복값·모두 같은 값입니다.
각 반복에서 같은 원본을 복사하여 세 정렬에 주며, 실행 순서를 순환합니다.

시간 측정 빌드에서는 연산 카운터를 제거했습니다. 시간은 중앙값 및 Q1~Q3로 집계하며, Q1~Q3는 신뢰구간이 아닙니다.
비교·이동 횟수는 별도의 카운터 빌드로 각 조건의 첫 측정 seed를 다시 실행한 값입니다.
`heap_buffer_bytes`는 알고리즘 내부의 **동적 보조 배열** 크기이며, 전체 프로세스 메모리나 호출 스택 사용량이 아닙니다.

## 실행 결과의 출처와 재측정 주의

첨부 결과는 2026-09-30에 **AI 지원 원격 Linux 실행 환경에서 실제 측정한 값**입니다. 제출자의 개인 컴퓨터에서 측정한 값으로 표시하지 않았습니다.
`results/environment.txt`에 환경을, `raw_timings.csv`에 630회 측정값을 기록했습니다. 알고리즘·환경에 따라 다른 실행에서는 시간이 달라집니다.

`make bench`는 `results/`의 CSV와 환경 파일, `report/figures/random_time.svg`를 덮어씁니다.
**보고서 DOCX/PDF/Markdown 및 PNG 그림의 수치는 자동 갱신되지 않습니다.** 재측정 결과를 보고서에 사용할 때에는 표·분석·PNG 그림·실행 환경까지 같은 실행 결과로 맞춘 뒤 PDF를 다시 생성해야 합니다. 원본 결과를 보관하려면 재측정 전에 프로젝트를 별도 복사하세요.

`make test`와 `make run`은 원본 측정 CSV를 덮어쓰지 않습니다. 기존 로그는 첨부된 최초 실행 기록입니다.

## 확인된 검사 결과

```text
SORT_COUNTING=1: 11980 checks, 0 failures
SORT_COUNTING=0: 11976 checks, 0 failures
ASan + UBSan: 11980 checks, 0 failures
```

두 빌드의 검사 대부분은 같은 사례를 반복한 것입니다. 이를 서로 독립된 서로 다른 테스트의 총합으로 해석하지 않습니다.
검사가 통과했다고 모든 가능한 입력에서의 무결성이 수학적으로 증명된 것은 아닙니다.

## 참고 자료와 AI 활용

- 과제 샘플: https://github.com/lec-algorithm/hw1-sample-2026
- 정렬 목록: https://en.wikipedia.org/wiki/Sorting_algorithm#Comparison_of_algorithms
- Princeton, Elementary Sorts: https://algs4.cs.princeton.edu/21elementary/
- Princeton, Mergesort: https://algs4.cs.princeton.edu/22mergesort/
- Princeton, Priority Queues: https://algs4.cs.princeton.edu/24pq/
- NIST, heapsort: https://xlinux.nist.gov/dads/HTML/heapSort.html

AI를 알고리즘 설명, 코드·검사 작성 지원, 실제 실행 및 보고서 정리에 활용했습니다. 학습 문답은 제출용으로 정리한 설명이며 실제 사용자의 질문 이력을 그대로 옮긴 채팅 로그는 아닙니다. 제출 전 코드와 힙 정렬 문답을 읽고 동작을 확인하세요.
