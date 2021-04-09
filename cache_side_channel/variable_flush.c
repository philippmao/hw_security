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

int main(){
  // a sufficently large test array
  char test_array[TEST_SIZE*4];
  uint64_t test1, test2, test3, test4, test5;
  unsigned int diff;
  // flushing all possible locations of the array
  _mm_clflush(&test_array[0]);
  _mm_clflush(&test_array[TEST_SIZE*3]);
  _mm_clflush(&test_array[TEST_SIZE*2]);
  _mm_clflush(&test_array[TEST_SIZE*1]);

  /*
  In this section we test the access time a specific array element
  whenever the access time is measured, the element will also be cached
  */
  diff = get_access_time(&test_array[TEST_SIZE*3]);
  printf("single time access time 1: %d\n", diff);
  // waiting for changes to take place
  for(int i=0;i<200000;i++){
    i++;
  }
  diff = get_access_time(&test_array[TEST_SIZE*2]);
  printf("access different item: %d\n", diff);
  diff = get_access_time(&test_array[TEST_SIZE*3]);
  printf("single time access time after access & wait: %d\n", diff);
  _mm_clflush(&test_array[TEST_SIZE*3]);
  diff = get_access_time(&test_array[TEST_SIZE*2]);
  printf("access different item: %d\n", diff);
  for(int i=0;i<100000;i++){
  }
  diff = get_access_time(&test_array[TEST_SIZE*3]);
  printf("single time access time after flush: %d\n", diff);
  diff = get_access_time(&test_array[TEST_SIZE*2]);
  printf("access different item: %d\n", diff);
  diff = get_access_time(&test_array[TEST_SIZE*3]);
  printf("access time after accessing another element: %d\n", diff);
}
