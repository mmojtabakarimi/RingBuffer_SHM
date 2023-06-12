#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>


#define BUFFER_SIZE 10

struct Request {
    int id;
    // Add any additional fields you need for the request
};

struct Response {
    int id;
    // Add any additional fields you need for the response
};

struct RingBuffer {
    Request requests[BUFFER_SIZE];
    Response responses[BUFFER_SIZE];
    unsigned int bufferSize;
    unsigned int mask;
    unsigned int writePos;
    unsigned int readPos;
    sem_t* producerSemaphore;
    sem_t* consumerSemaphore;
};
