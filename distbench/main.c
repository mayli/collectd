/* System headers */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if HAVE_CLOCK_GETTIME
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <unistd.h>

/* Local headers */
#include "function.h"

/* Macro to exit with an error. */
#define error(fmt, args...)                                                    \
  do {                                                                         \
    fprintf(stderr, "ERROR: " fmt "\n", ##args);                               \
    _exit(1);                                                                  \
  } while (0)

/* How many nanoseconds there are in a second. */
#define NANOS_PER_SECOND 10000000

/* How many microseconds there are in a nanosecond. */
#define MICROS_PER_NANO 1000

/* How many iterations to run. */
#define ITERATIONS 10000000

/* Returns the clock in nanoseconds. */
static uint64_t get_clock() {
#if HAVE_CLOCK_GETTIME
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    error("Unable to retrieve monotonic clock: %s", strerror(errno));

  return (ts.tv_sec * NANOS_PER_SECOND) + ts.tv_nsec;
#else
  struct timeval tv;

  if (gettimeofday(&tv, NULL) == -1)
    error("Unable to retrieve current time: %s", strerror(errno));

  return (tv.tv_sec * NANOS_PER_SECOND) + (tv.tv_usec * MICROS_PER_NANO);
#endif
}

int main(int argc, char **argv) {
  uint64_t start = get_clock();

  for (int i = 0; i < ITERATIONS; i++) {
    /* Store result in a volatile to prevent the compiler from ignoring the call
     * because the result is unused. */
    volatile int result = my_function();

    /* Prevent "unused variable" warning/error. */
    (void)result;
  }

  uint64_t end = get_clock();
  uint64_t duration = end - start;

  printf("%s: %d iterations took %f seconds (~ %d nanoseconds per call)\n",
         argv[0],    /* Program name */
         ITERATIONS, /* Number of iterations */
         (duration * 1.0 /
          NANOS_PER_SECOND), /* *1.0 to force floating-point arithmetic */
         (int)(duration / ITERATIONS)); /* Only integer division here */
}