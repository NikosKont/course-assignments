Nikolaos Kontogeorgis, csd4655

Each student thread has its own semaphore. The first 8 students immediately get in the study room.
The next students will wait in the waiting room. When a student leaves the study room, their seat is empty
but only when the last student leaves the study room, their thread will post the semaphores of the next 8
students in the waiting room (if there are any, or the remaining ones if they are less than 8).
The students always get in the study room in the same order they arrived in the waiting room.