#include<stdio.h>
#include<stdint.h>
#include <x86intrin.h>
#include <unistd.h>



/* measures access times over  */
unsigned int get_access_time(volatile char *addr){
  unsigned long long time1, time2;
  char test;
  time1 = __rdtsc();
  test = *addr;
  _mm_mfence();
  time2 = __rdtsc();
  return time2 - time1;
}

int TEST_SIZE = 4096;
int TARGET = 69;
int EXPERIMENTS = 1000;

/*
run the flush reload experiment for 1000 tries. Calculate the average access
time for each element, the target element is the one with the lowest access time.
*/

int main(int argc, char** argv){
  printf("argc: %d\n", argc);
  if(argc == 2){
    TARGET = atoi(argv[1]);
  }
  // a sufficently large test array
  char test_array[TEST_SIZE*256];
  char junk_array[TEST_SIZE*5];
  unsigned int result_times[256];
  for(int i=0; i<256; i++){
    result_times[i] = 0;
  }
  char junk;
  unsigned int diff, j;
  for(int k=0;k<EXPERIMENTS;k++){
    // flushing all possible locations of the array
    for(int i=0;i<256;i++){
      _mm_clflush(&test_array[i*TEST_SIZE]);
    }
    // waiting for changes to take place
    for(int i=0;i<200000;i++){
      i++;
    }
    /*
    Try accessing the target elent
    */
    junk = test_array[TEST_SIZE*TARGET];
    // waiting for changes to take place
    for(int i=0;i<200000;i++){
      i++;
    }
    junk_array[0] = "a";
    for(int i=0;i<256;i++){
      j = ((i*167) + 13) & 255;
      diff = get_access_time(&test_array[TEST_SIZE*j]);
      result_times[j] += diff;
    }
  }
  // extract the value with the minimum access time
  int min, min2;
  min = 0;
  min2 = 0;
  for(int i=0;i<256;i++){
    if(result_times[i] < result_times[min]){
      min = i;
    } else {
      if(result_times[i] < result_times[min2]){
        min2 = i;
      }
    }
  }
  int avg = result_times[min] /EXPERIMENTS;
  printf("minimum value: %d, average access time: %d\n", min, avg);
  avg = result_times[min2] /EXPERIMENTS;
  printf("second minimum value: %d, average access time: %d\n", min2, avg);
}
