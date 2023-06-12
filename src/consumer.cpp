

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

    // Open the producer semaphore
    ringBuffer->producerSemaphore = sem_open("/producer_semaphore", 0);
    if (ringBuffer->producerSemaphore == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Create and open the consumer semaphore
    sem_unlink("/consumer_semaphore"); // Remove any existing semaphore with the same name
    ringBuffer->consumerSemaphore = sem_open("/consumer_semaphore", O_CREAT | O_EXCL, 0666, 0);
    if (ringBuffer->consumerSemaphore == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    while (true) {
        sem_wait(ringBuffer->consumerSemaphore);

        // Calculate the read position
        unsigned int readPos = ringBuffer->readPos;
        unsigned int nextReadPos = (readPos + 1) & ringBuffer->mask;

        // Consume the request from the buffer
        Request request = ringBuffer->requests[readPos];

        // Generate the response
        Response response;
        response.id = request.id;
        // Populate the response with the necessary data

        // Process the response or send it to the appropriate handler
        std::cout << "Consumer: Generated response for request " << response.id << std::endl;

        // Publish the response to the buffer
        ringBuffer->responses[readPos] = response;
        __sync_synchronize();
        ringBuffer->readPos = nextReadPos;

        sem_post(ringBuffer->producerSemaphore);
    }

    return 0;
}
