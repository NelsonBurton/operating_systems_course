/* Introduction to Operating Systems
   
  == Problem Set 1 ==
  Priority Readers and Writers
    Write a multi-threaded C program that gives readers priority over writers concerning a shared (global) variable. Essentially, if any readers are waiting, then they have priority over writer threads -- writers can only write when there are no readers. This program should adhere to the following constraints:
      Multiple readers/writers must be supported (5 of each is fine)
      Readers must read the shared variable X number of times
      Writers must write the shared variable X number of times
      Readers must print:
        The value read
        The number of readers present when value is read
      Writers must print:
        The written value
        The number of readers present were when value is written (should be 0)
      Before a reader/writer attempts to access the shared variable it should wait some random amount of time
        Note: This will help ensure that reads and writes do not occur all at once
      Use pthreads, mutexes, and condition variables to synchronize access to the shared variable

*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define X 5    /* Number each reader/writer thread repeats */
#define NUM_THREADS 5
int sharedVar = 0;

int resource_counter = 0;

pthread_mutex_t m_rc = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER;

void *reader (void *param);
void *writer (void *param);

int main(int argc, char *argv[]) {
  printf("In Main Method\n");
  fflush(stdout);

  int i;

  int readerNum[NUM_THREADS];
  int writerNum[NUM_THREADS];

  pthread_t readerThreadIDs[NUM_THREADS];
  pthread_t writerThreadIDs[NUM_THREADS];

  // Start the readers
  for(i = 0; i < NUM_THREADS; i++) {
    readerNum[i] = i;
    pthread_create(&readerThreadIDs[i], NULL, reader, &readerNum[i]);
    writerNum[i] = i;
    pthread_create(&writerThreadIDs[i], NULL, writer, &writerNum[i]);
  }

  // Wait on readers & writers to finish
  for(i = 0; i < NUM_THREADS; i++) {
    pthread_join(readerThreadIDs[i], NULL);
    pthread_join(writerThreadIDs[i], NULL);
  }

  printf("Parent quiting\n");
  fflush(stdout);

  return 0;
}

/* Read Values; there can be multiple readers at any time */
void *reader(void *param) {
  
  int thread_id = *((int*)param);
  int i, random_ms_to_wait;

  for (i=0; i<X; i++) {
    random_ms_to_wait = rand() % 20;
    usleep(random_ms_to_wait*1000); //sleep for 0-19 milliseconds
    
    pthread_mutex_lock (&m_rc);
      while (resource_counter == -1) { // while write happening, wait
        pthread_cond_wait(&c_read, &m_rc);
      }
      resource_counter++; // increment num of reader threads
    pthread_mutex_unlock (&m_rc);

    // read data
    fprintf (stdout, "reader%d: reading %d with %d readers present\n",thread_id, sharedVar, resource_counter);

    pthread_mutex_lock (&m_rc);
      resource_counter--;
      if (resource_counter == 0) {
        pthread_cond_signal(&c_write);
      }
    pthread_mutex_unlock (&m_rc);
  }
  return 0;
}

/* Write Value; only 1 writer is allowed to write at a time */
void *writer(void *param) {
  
  int thread_id = *((int*)param);
  int i, random_ms_to_wait;

  for (i=0; i<X; i++) {
    random_ms_to_wait = rand() % 20;
    usleep(random_ms_to_wait*1000); //sleep for 0-19 milliseconds

    pthread_mutex_lock (&m_rc);
      while (resource_counter != 0) { // wait while read/write happening
        pthread_cond_wait(&c_write, &m_rc);
      }
      resource_counter = -1; // block readers
    pthread_mutex_unlock (&m_rc);

    // write data
    sharedVar++;
    fprintf (stdout, "writer%d: writing %d with %d readers present\n",thread_id, sharedVar, resource_counter);

    pthread_mutex_lock (&m_rc);
      resource_counter = 0;
      pthread_cond_broadcast(&c_read); // tell all readers go
      pthread_cond_signal(&c_write);  // tell one writer go
    pthread_mutex_unlock (&m_rc);
  }

  return 0;
}

