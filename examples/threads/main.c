#include <stdio.h>

#include "thread.h"
#include "xtimer.h"
#include "msg.h"

kernel_pid_t main_pid;
void *second_thread(void *arg)
{
    (void) arg;

    printf("2nd thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;
    msg_t m1;

    while(1) {
        msg_receive(&m);
        printf("\nreceived 2.\n");
        xtimer_sleep(2);
        m1.sender_pid = main_pid;
        m1.content.value = 2;
        msg_reply(&m1, &m1);
    }
    
    return NULL;
}

char second_thread_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
    printf("Starting IPC Ping-pong example...\n");
    printf("1st thread started, pid: %" PRIkernel_pid "\n", thread_getpid());

    msg_t m;
    main_pid = thread_getpid();
    kernel_pid_t pid = thread_create(second_thread_stack, sizeof(second_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                            second_thread, NULL, "pong");

    m.content.value = 1;

    // msg_send(&m, pid);
    // printf("\nsent.\n");
    // msg_receive(&m);
    // printf("\nreceived 1.\n");
    // while (1) {
        msg_send_receive(&m, &m, pid);
        printf("1st: Got msg with content %u\n", (unsigned int)m.content.value);
    // }
}