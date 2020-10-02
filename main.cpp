#include <iostream>
#include <pthread.h>
#include <time.h>


#define MAX 20
#define THREAD_MAX 4

using namespace std;

int ARR[MAX];
int part = 0;

void merge(int low, int mid, int high) {
    int* left = new int[mid - low + 1];
    int* right = new int[high - mid];

    int size_left = mid - low + 1;
    int size_right = high - mid;
    int i, j;

    for (i = 0; i < size_left; i++)
        left[i] = ARR[i + low];

    for (i = 0; i < size_right; i++)
        right[i] = ARR[i + mid + 1];

    int k = low;
    i = j = 0;

    while (i < size_left && j < size_right) {
        if (left[i] <= right[j])
            ARR[k++] = left[i++];
        else
            ARR[k++] = right[j++];
    }

    while (i < size_left) {
        ARR[k++] = left[i++];
    }

    while (j < size_right) {
        ARR[k++] = right[j++];
    }
}

void merge_sort(int low, int high) {

    int mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid);

        merge_sort(mid + 1, high);

        merge(low, mid, high);
    }
}

void* merge_sort(void* arg) {

    int thread_part = part++;

    int low = thread_part * (MAX / 4);
    int high = (thread_part + 1) * (MAX / 4) - 1;
    int mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

int main() {
    for (int i = 0; i < MAX; i++)
        ARR[i] = rand() % 100;

    clock_t t1, t2;

    t1 = clock();
    pthread_t threads[THREAD_MAX];

    for (int i = 0; i < THREAD_MAX; i++)
        pthread_create(&threads[i], NULL, merge_sort, (void*)NULL);

    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    merge(0, (MAX / 2 - 1) / 2, MAX / 2 - 1);
    merge(MAX / 2, MAX / 2 + (MAX - 1 - MAX / 2) / 2, MAX - 1);
    merge(0, (MAX - 1) / 2, MAX - 1);

    t2 = clock();

    cout << "Sorted array: ";
    for (int i = 0; i < MAX; i++)
        cout << ARR[i] << " ";

    cout << "\nTime taken: " << (t2 - t1) / (double)CLOCKS_PER_SEC << endl;

    return 0;
}