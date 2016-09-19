#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static int perfEventOpen(struct perf_event_attr* hw_event,
                         pid_t pid,
                         int cpu,
                         int group_fd,
                         unsigned long flags)
{
    int result = 0;

    result = syscall(__NR_perf_event_open, hw_event,
                     pid, cpu, group_fd, flags);
    return result;
}

static unsigned long sumNormal(unsigned long* vec, size_t size)
{
    unsigned long result = 0;

    for (size_t i = 0; i < size; i++)
    {
        result += vec[i];
    }
    return result;
}

static unsigned long* createBuffer(size_t size)
{
    unsigned long* result = 0;

    result = (unsigned long*)malloc(sizeof(unsigned long) * size);
    if (result != NULL)
    {
        srand((unsigned)time(NULL));
        for (size_t i = 0; i < size; i++)
        {
            result[i] = rand();
        }
    }
    return result;
}

static void freeBuffer(unsigned long* buffer)
{
    if (buffer != NULL)
    {
        free(buffer);
        buffer = NULL;
    }
}

int main(void)
{
    struct perf_event_attr attr;
    int perf_fd;
    long long val0 = 0, val1 = 0;
    long long tsc0 = 0, tsc1 = 0;

    memset(&attr, 0, sizeof(attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;

    perf_fd = perfEventOpen(&attr, 0, -1, -1, 0);
    if (perf_fd == -1)
    {
        perror("perfEventOpen");
        exit(1);
    }

    {
        for (int i = 0; i < 100; i++)
        {
            unsigned long* buffer = NULL;
            unsigned long result = 0;

            buffer = createBuffer(1024 * 1024);

            read(perf_fd, &val0, sizeof(val0));
//            tsc0 = __rdtsc();

//            result = sumNormal(buffer, 1024 * 1024);

//            tsc1 = __rdtsc();
            read(perf_fd, &val1, sizeof(val1));

            freeBuffer(buffer);

            printf("result = %lu, CPU_CLK_UNHALTED = %lld, tsc = %lld\n",
                result, val1 - val0, tsc1 - tsc0);
        }
    }

    close(perf_fd);

}

