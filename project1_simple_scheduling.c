// Round Robin, 타이머, IPC, fork 등에 필요한 헤더들
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>

// 부모가 자식에게 보내는 기본 코드

struct msgbuf {
    long mtype;
    int value;
};

void send_timeslice(int msgid, pid_t child_pid) {
struct msgbuf msg;

msg.mtype = child_pid; // 이 메시지는 child_pid에게 배달된다.
msg.value = 1; //CPU time slice = 1 tick
msgsnd(msgid,&msg,sizeof(msg.value),0);
}

// Child Code
void exe_child(int msgid) {
    struct msgbuf msg;

    int cpu_burst = rand() % 5 + 1;  // CPU burst 랜덤 시작값 (1~5)

    while (1) {
        // 1) 부모가 보내는 메시지를 기다림
        msgrcv(msgid, &msg, sizeof(msg.value), getpid(), 0);

        // 2) 메시지를 받으면 CPU burst를 1 감소
        cpu_burst--;

        // 3) CPU burst가 끝났으면 부모에게 IO 요청 메시지 보내기
        if (cpu_burst <= 0) {
            struct msgbuf send;
            send.mtype = 1;                // 부모가 받을 타입
            send.value = rand() % 5 + 1;   // IO burst 랜덤 생성

            msgsnd(msgid, &send, sizeof(send.value), 0);

            // 다음 CPU burst도 랜덤으로 생성
            cpu_burst = rand() % 5 + 1;
        }
    }
}


int main() {

    // 1) 메시지 큐 생성
    int msgid = msgget((key_t)1234, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // 2) child PID 저장 배열
    pid_t pids[10];

    // 3) 10개 child 생성
    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            // ---- child code ----
            exe_child(msgid);
            exit(0);
        } else {
            // parent에 저장
            pids[i] = pid;
        }
    }

    // ---- 부모가 여기서 스케줄링 루프 돌게 될 예정 ----
    while (1) {
        send_timeslice(msgid, pids[0]); // child0에게 계속 CPU 주기 (시험용으로 이렇게 잠깐 써둠.)
        sleep(1); // 임시
    }

    return 0;
}
