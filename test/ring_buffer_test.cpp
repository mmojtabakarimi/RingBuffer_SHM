#include <gtest/gtest.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include "../include/ring_buffer.h" // Include the ring buffer header

// Define the path to the producer and consumer executables
const char* PRODUCER_EXECUTABLE = "../src/producer";
const char* CONSUMER_EXECUTABLE = "../src/consumer";

class RingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create shared memory segment
        key_t key = IPC_PRIVATE; // Generate a unique key for shared memory
        if ((shmid = shmget(key, sizeof(RingBuffer), IPC_CREAT | 0666)) < 0) {
            perror("shmget");
            exit(1);
        }

        // Attach shared memory segment
        ringBuffer = static_cast<RingBuffer*>(shmat(shmid, nullptr, 0));
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

        // Start the consumer process
        pid_t pid = fork();
        if (pid == 0) {
            // Child process, execute the consumer
            execl(CONSUMER_EXECUTABLE, NULL);
            exit(0);
        }
    }

    void TearDown() override {
        // Clean up shared memory and semaphores
        shmdt(ringBuffer);
        shmctl(shmid, IPC_RMID, nullptr);
        sem_unlink("/producer_semaphore");
        sem_unlink("/consumer_semaphore");
    }

    int shmid;
    RingBuffer* ringBuffer;
};

TEST_F(RingBufferTest, ProducerConsumerInteraction) {
    // Simulate producer and consumer interaction
    // ...
    // Add your test assertions here
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}