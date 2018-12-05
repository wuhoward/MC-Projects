// See LICENSE for license details.

#ifndef __UTIL_H
#define __UTIL_H

//--------------------------------------------------------------------------
// Macros

// Set HOST_DEBUG to 1 if you are going to compile this for a host
// machine (ie Athena/Linux) for debug purposes and set HOST_DEBUG
// to 0 if you are compiling with the smips-gcc toolchain.

#ifndef HOST_DEBUG
#define HOST_DEBUG 0
#endif

// Set PREALLOCATE to 1 if you want to preallocate the benchmark
// function before starting stats. If you have instruction/data
// caches and you don't want to count the overhead of misses, then
// you will need to use preallocation.

#ifndef PREALLOCATE
#define PREALLOCATE 0
#endif

// Set SET_STATS to 1 if you want to carve out the piece that actually
// does the computation.

#if HOST_DEBUG
#include <stdio.h>
static void setStats(int enable) {}
#else
extern void setStats(int enable);
#endif

#include <stdint.h>
#include <stddef.h>
#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }

static void printArray(const char name[], int n, const int arr[])
{
#if HOST_DEBUG
  int i;
  printf( " %10s :", name );
  for ( i = 0; i < n; i++ )
    printf( " %3d ", arr[i] );
  printf( "\n" );
#endif
}

static void printDoubleArray(const char name[], int n, const double arr[])
{
#if HOST_DEBUG
  int i;
  printf( " %10s :", name );
  for ( i = 0; i < n; i++ )
    printf( " %g ", arr[i] );
  printf( "\n" );
#endif
}

static int verify(int n, const volatile int* test, const int* verify)
{
  int i;
  // Unrolled for faster verification
  for (i = 0; i < n/2*2; i+=2)
  {
    int t0 = test[i], t1 = test[i+1];
    int v0 = verify[i], v1 = verify[i+1];
    if (t0 != v0) return i+1;
    if (t1 != v1) return i+2;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static int verifyDouble(int n, const volatile double* test, const double* verify)
{
  int i;
  // Unrolled for faster verification
  for (i = 0; i < n/2*2; i+=2)
  {
    double t0 = test[i], t1 = test[i+1];
    double v0 = verify[i], v1 = verify[i+1];
    int eq1 = t0 == v0, eq2 = t1 == v1;
    if (!(eq1 & eq2)) return i+1+eq1;
  }
  if (n % 2 != 0 && test[n-1] != verify[n-1])
    return n;
  return 0;
}

static void __attribute__((noinline)) barrier(int ncores)
{
  static volatile int sense;
  static volatile int count;
  static __thread int threadsense;

  __sync_synchronize();

  threadsense = !threadsense;
  if (__sync_fetch_and_add(&count, 1) == ncores-1)
  {
    count = 0;
    sense = threadsense;
  }
  else while(sense != threadsense)
    ;

  __sync_synchronize();
}

typedef struct {
  int state;
} Mutex;


#define mutex_is_locked(x)    ((x)->state != 0)

static inline void mutex_unlock(volatile Mutex *mutex)
{
  __asm__ __volatile__ (
      "amoswap.w.rl x0, x0, %0"
      : "=A" (mutex->state)
      :: "memory"
      );

}

static inline int trylock(volatile Mutex *mutex)
{
      int tmp = 1, busy;

      __asm__ __volatile__ (
          "amoswap.w.aq %0, %2, %1"
          : "=r" (busy), "+A" (mutex->state)
          : "r" (tmp)
          : "memory"
          );

          return !busy;

}

static inline void mutex_lock(volatile Mutex *mutex)
{
  while (1) {
    if (mutex_is_locked(mutex))
      continue;

    if (trylock(mutex))
      break;

  }

}

static uint64_t lfsr(uint64_t x)
{
  uint64_t bit = (x ^ (x >> 1)) & 1;
  return (x >> 1) | (bit << 62);
}

//simplified ilib

#define QUEUE_SIZE 10000
#define MAX_CORE 10

static Mutex qm;
typedef struct myqueue{
    char recv_buf[QUEUE_SIZE][100];  
    int recv_rank[QUEUE_SIZE];       
    int head;     
    int tail;      
}queue;

static queue mem[MAX_CORE];

static int get_rank(){
  int r;
  __asm__ __volatile__ (
          "csrr %0, mhartid"
          : "=r" (r)
          ::"memory"
          );
  return r;
}

static void ilib_send(int rank, const void* buffer, size_t size){
  int myrank = get_rank();
  mutex_lock(&qm);
  memcpy(mem[rank].recv_buf[mem[rank].head], buffer, size);
  mem[rank].recv_rank[mem[rank].head] = myrank;
  mem[rank].head = (mem[rank].head+1)%QUEUE_SIZE;
  mutex_unlock(&qm);
}

static int ilib_recv(const void* buffer, size_t size){
  int sender;
  int rank = get_rank();
  printf("");
  while(1){
    if(mem[rank].tail != mem[rank].head){
      memcpy(buffer, mem[rank].recv_buf[mem[rank].tail], size);
      sender = mem[rank].recv_rank[mem[rank].tail];
      mem[rank].tail = (mem[rank].tail+1)%QUEUE_SIZE;
      return sender;
    }
  } 
}

#ifdef __riscv
#include "encoding.h"
#endif

#define stringify_1(s) #s
#define stringify(s) stringify_1(s)
#define stats(code, iter) do { \
    unsigned long _c = -read_csr(mcycle), _i = -read_csr(minstret); \
    code; \
    _c += read_csr(mcycle), _i += read_csr(minstret); \
    if (cid == 0) \
      printf("\n%s: %ld cycles, %ld.%ld cycles/iter, %ld.%ld CPI\n", \
             stringify(code), _c, _c/iter, 10*_c/iter%10, _c/_i, 10*_c/_i%10); \
  } while(0)

#endif //__UTIL_H
