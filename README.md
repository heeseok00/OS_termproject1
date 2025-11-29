# OS Term Project #1: Round-Robin 스케줄링 구현

운영체제 과제로 구현한 Round-Robin 스케줄링 시뮬레이터입니다. 부모 프로세스가 여러 자식 프로세스를 관리하며 CPU 시간을 할당하고, IO 작업을 처리하는 과정을 시뮬레이션합니다.

## 프로젝트 개요

이 프로젝트는 다음과 같은 기능을 구현합니다:

- Round-Robin 스케줄링 알고리즘
- 부모-자식 프로세스 간 IPC 통신 (메시지 큐)
- Run Queue와 Wait Queue를 통한 프로세스 상태 관리
- 타이머 기반 시간 틱 시스템
- CPU burst와 IO burst 처리
- 스케줄링 과정 로그 기록

## 주요 기능

### 스케줄링 알고리즘

Round-Robin 방식으로 각 프로세스에 고정된 타임 퀀텀(3 ticks)을 할당합니다. 타임 퀀텀이 소진되면 다음 프로세스로 전환하고, 현재 프로세스는 run queue의 맨 뒤로 이동합니다.

### 프로세스 관리

10개의 자식 프로세스가 생성되며, 각 프로세스는 다음과 같은 동작을 수행합니다:

- CPU burst: CPU를 사용하여 작업 수행
- IO burst: CPU burst 완료 후 IO 작업 요청
- IO 완료 후 새로운 CPU burst 시작

### 큐 관리

두 가지 큐를 사용하여 프로세스 상태를 관리합니다:

- **Run Queue**: CPU를 사용할 준비가 된 프로세스들
- **Wait Queue**: IO 작업을 수행 중인 프로세스들

### 로그 시스템

매 시간 틱마다 다음 정보를 `schedule_dump.txt` 파일에 기록합니다:

- 현재 시간 (tick)
- 실행 중인 프로세스 ID
- 남은 CPU burst
- Run Queue 상태
- Wait Queue 상태

## 컴파일 및 실행

```bash
gcc -o project1_simple_scheduling project1_simple_scheduling.c
./project1_simple_scheduling
```

실행 후 `schedule_dump.txt` 파일이 생성되며, 10,000개의 시간 틱 동안의 스케줄링 과정이 기록됩니다.

## 주요 상수

- `MAX_TICKS`: 10000 (총 실행 시간 틱 수)
- `TICK_INTERVAL_USEC`: 10000 (10ms, 각 틱의 시간 간격)
- `TIME_QUANTUM`: 3 (각 프로세스에 할당되는 타임 퀀텀)
- `MAX_CHILDREN`: 10 (생성되는 자식 프로세스 수)
- `QUEUE_SIZE`: 100 (큐의 최대 크기)

## 파일 구조

- `project1_simple_scheduling.c`: 메인 소스 코드
- `schedule_dump.txt`: 실행 결과 로그 파일 (실행 후 생성)
- `README.md`: 프로젝트 설명서

## 구현 세부사항

### 초기화 단계

1. 메시지 큐 생성 및 로그 파일 열기
2. 10개의 자식 프로세스 생성 및 run queue 초기화
3. 타이머 설정 (setitimer) 및 SIGALRM 핸들러 등록

### 스케줄링 루프

각 시간 틱마다 다음 순서로 동작합니다:

1. 이전 틱에서 실행 중이던 프로세스 처리 (타임 퀀텀 감소, 재등록)
2. Run queue에서 다음 프로세스 선택 및 CPU 할당
3. 자식 프로세스의 IO 요청 수신 및 wait queue 이동
4. Wait queue의 IO burst 감소 및 완료된 프로세스 run queue 복귀
5. 현재 상태 로그 파일에 기록

### IPC 통신

부모와 자식 프로세스 간 통신은 System V 메시지 큐를 사용합니다:

- 부모 → 자식: CPU time slice 전송
- 자식 → 부모: IO 요청 메시지 전송

## 참고사항

- 이 프로젝트는 Linux/Unix 환경에서 실행됩니다
- 메시지 큐는 프로세스 종료 시 자동으로 정리됩니다
- 모든 자식 프로세스는 부모 프로세스 종료 전에 정상적으로 종료됩니다

