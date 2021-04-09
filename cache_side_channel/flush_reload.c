/* implements simple flush and reload POC */
#include<stdio.h>
#include<stdint.h>
#include <x86intrin.h>
#include <unistd.h>

int PAGE_SIZE = 4096;
int CACHE_THRESHOLD = 70;

unsigned int get_access_time(volatile char *addr){
  unsigned long long time1, time2;
  char test;
  time1 = __rdtsc();
  test = *addr;
  _mm_mfence();
  time2 = __rdtsc();
  return time2 - time1;
}

/* measures access times over  */
void measure_access(char *probe_array, unsigned int *measurements){
  volatile char *addr;
  int j;
  for(int i=0; i < 256; i++){
    //TODO irregular access to avoid stride detection
    j = (i*13+35) % 256;
    addr = &probe_array[j*4096];
    measurements[j] = get_access_time(addr);
  }
}

void get_cache_threshold(char *probe_array){
  int ESTIMATE_CYCLE = 1000000;
  int cached, uncached, i;
  for (cached = 0, i=0; i < ESTIMATE_CYCLE; i++){
    cached += get_access_time(&probe_array[0]);
  }
  for (uncached = 0, i=0; i < ESTIMATE_CYCLE; i++){
    _mm_clflush(&probe_array[0]);
    uncached += get_access_time(&probe_array[0]);
  }
  printf("cache time: %d, uncached time: %d\n", cached, uncached);
  cached /= ESTIMATE_CYCLE;
  uncached /= ESTIMATE_CYCLE;
  printf("cache time: %d, uncached time: %d\n", cached, uncached);
}


int main(){
  static char probe_array[256*4096];

  unsigned int measurements[256];
  unsigned int diff;



  get_cache_threshold(probe_array);
/*
  char test1 = 15;
  char test2 = 1;
  char test3 = 24;

  _mm_clflush(&test1);
  _mm_clflush(&test2);
  _mm_clflush(&test3);
  _mm_mfence();
  diff = get_access_time(&test1);
  printf("access time test1: %d\n", diff);
  diff = get_access_time(&test1);
  printf("access time test1: %d\n", diff);
  diff = get_access_time(&test2);
  printf("access time test1: %d\n", diff);
  diff = get_access_time(&test3);
  printf("access time test1: %d\n", diff);
*/


  char test = probe_array[0*4096];
  sleep(1);
  diff = get_access_time(&probe_array[0*4096]);
  printf("access time test1: %d\n", diff);

  //_mm_clflush(&probe_array[48*4096]);

  diff = get_access_time(&probe_array[0*4096]);
  printf("access time test2: %d\n", diff);
  diff = get_access_time(&probe_array[0*4096]);
  printf("access time test2: %d\n", diff);
  diff = get_access_time(&probe_array[0*4096]);
  printf("access time test2: %d\n", diff);
  diff = get_access_time(&probe_array[0*4096]);
  printf("access time test2: %d\n", diff);
/*
  unsigned int results[256];
  int EXPERIMENTS = 1;
  for(int i=0; i<256; i++){
    results[i] = 0;
  }
  for(int j=0; j<EXPERIMENTS; j++){

    //flush the target array
    for(int i=0; i<256; i++){
      _mm_clflush(&probe_array[i*4096]);
    }
    //access the secret byte
    (void)probe_array[48*4096];

    measure_access(probe_array, measurements);

    // record results into the results array
    for(int i=0; i<256; i++){
      if(measurements[i] < CACHE_THRESHOLD){
        results[i] = results[i] + 1;
      }
    }
  }
  for(int i=0; i<256; i++){
    printf("result of %d, %d\n", i, results[i]);
  }
  for(int i=0; i<256; i++){
    printf("access time of %d, %d\n", i, measurements[i]);
  }

  for(int i=0; i<256; i++){
    _mm_clflush(&probe_array[i*4096]);
  }

  (void)probe_array[48*4096];
  diff = get_access_time(&probe_array[48*4096]);
  printf("access time test1: %d\n", diff);
  diff = get_access_time(&probe_array[150*4096]);
  printf("access time test2: %d\n", diff);*/
  return 0;
}
