
#include "ring_buffer.h"


int main() {
    int shmid;
    key_t key = IPC_PRIVATE; // Generate a unique key for shared memory

    // Create shared memory segment
    if ((shmid = shmget(key, sizeof(RingBuffer), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory segment
    RingBuffer* ringBuffer = static_cast<RingBuffer*>(shmat(shmid, nullptr, 0));
    if (ringBuffer == reinterpret_cast<RingBuffer*>(-1)) {
        perror("shmat");
        exit(1);
    }

    ringBuffer->bufferSize = BUFFER_SIZE;
    ringBuffer->mask = BUFFER_SIZE - 1;
    ringBuffer->writePos = 0;
    ringBuffer->readPos = 0;

    // Create and open the producer semaphore
    sem_unlink("/producer_semaphore"); // Remove any existing semaphore with the same name
    ringBuffer->producerSemaphore = sem_open("/producer_semaphore", O_CREAT | O_EXCL, 0666, BUFFER_SIZE);
    if (ringBuffer->producerSemaphore == SEM_FAILED) {
        perror("1sem_open");
        exit(1);
    }

    // Open the consumer semaphore
    ringBuffer->consumerSemaphore = sem_open("/consumer_semaphore", 0);
    if (ringBuffer->consumerSemaphore == SEM_FAILED) {
        perror("2sem_open");
        exit(1);
    }

    // Producer code
    int requestId = 1;

    while (true) {
        sem_wait(ringBuffer->producerSemaphore);

        // Calculate the write position
        unsigned int writePos = ringBuffer->writePos;
        unsigned int nextWritePos = (writePos + 1) & ringBuffer->mask;

        // Produce the request
        Request request;
        request.id = requestId;
        // Populate the request with the necessary data

        // Publish the request to the buffer
        ringBuffer->requests[writePos] = request;
        __sync_synchronize();
        ringBuffer->writePos = nextWritePos;

        std::cout << "Producer: Added request " << requestId << " to the buffer" << std::endl;

        requestId++;

        sem_post(ringBuffer->consumerSemaphore);

        usleep(10000); // Sleep for 100ms
    }

    return 0;
}
