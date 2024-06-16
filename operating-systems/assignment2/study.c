// Nikolaos Kontogeorgis - csd4655

// #define _POSIX_C_SOURCE 200112L
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>

int N;
int cap = 8;
int studyroom[8];
int ids[40];
int waitroom[40];
sem_t semaphore[40];
pthread_t students[40];
pthread_mutex_t lock;

// used to keep track of order of students
int next = -1;
int last = 0;

// let the next 8 (or remaining if less than 8) students in
void letnext() {
    for (int i = 0; i < 8 && next <= last-1; i++, next++, cap--) {
        int id = waitroom[next];
        sem_post(&semaphore[id - 1]);
        printf("Student %02d enters the studying room\n", id);
        studyroom[i] = waitroom[next];
    }
}

void print_rooms() {
    printf("Studying room:\t");
    for (int i = 0; i < 8; i++) {
        printf("| ");
        if (studyroom[i] == 0)
            printf("   ");
        else
            printf("%02d ", studyroom[i]);
    }
    printf("|\n");

    printf("Waiting room:\t");
    for (int i = next; i < 40; i++) {
        if (waitroom[i] != 0)
            printf("| %02d ", waitroom[i]);
    }
    printf("|\n\n");
}

// thread function for a student
void* student(void* id_p) {
    int t = rand() % 20;
    sleep(t);

    int id = (int)id_p;

    // ensure synchronization
    pthread_mutex_lock(&lock);

    printf("Student %02d wants to study\n", id);

    // if in first 8 students, let them in
    if (cap > 0 && next == -1) {
        sem_post(&semaphore[id - 1]);
        studyroom[8-cap] = id;
        cap--;
        print_rooms();

        pthread_mutex_unlock(&lock);
    }
    else {
        printf("Student %02d is waiting to enter the studying room\n\n", id);
        if (next == -1) next = 0;
        waitroom[last++] = id;
        print_rooms();

        pthread_mutex_unlock(&lock);
        sem_wait(&semaphore[id - 1]);
    }

    // study...
    t = rand() % 11 + 5;
    sleep(t);

    pthread_mutex_lock(&lock);

    // leave study room
    for (int i = 0; i < 8; i++) {
        if (studyroom[i] == id) {
            studyroom[i] = 0;
            break;
        }
    }
    printf("Student %02d has left after studying for %d secs\n", id, t);
    print_rooms();
    cap++;

    // if study room is empty, let the next 8 (or remaining) students in
    if (cap == 8 && next <= last-1) {
        letnext();
        print_rooms();
    }

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(void) {
    printf("Please enter the number of students N: ");
    scanf("%d", &N);
    if (N < 20 || N > 40) {
        printf("Invalid N\n");
        exit(1);
    }

    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < N; i++)
        sem_init(&semaphore[i], 0, 0);

    for (int i = 0; i < N; i++) {
        int id = i + 1;
        ids[i] = id;
        pthread_create(&students[i], NULL, &student, (void*)id);
    }

    for (int i = 0; i < N; i++)
        pthread_join(students[i], NULL);

    for (int i = 0; i < 40; i++)
        sem_destroy(&semaphore[i]);

    pthread_mutex_destroy(&lock);

    printf("All students have studied and left the studying room\n");

    return 0;
}