#include <pthread.h>
#include <sys/time.h>
#include <fstream>
#include <string>
#include <stdlib.h>

long int * g_size_arr = nullptr;

struct params {
    long int id_th;
    long int num_th;
    long int *arr;
};

void merge_sort(long int arr[], long int left, long int right);
void merge(long int arr[], long int left, long int middle, long int right);
void *thread_merge_sort(void* arg);
void merge_sections(long int arr[], long int num_thread, long int size_sub_arr, long int aggregation);
void test_order(long int arr[]);

int main(int argc, char *argv[]) {
    long int g_num_threads = atoi(argv[1]);

    g_size_arr = new long int;
    printf("\n\tCount elements:\t");
    scanf("%ld", g_size_arr);
    long int *arr = new long int[*g_size_arr];


    long int size_sub_arr = *g_size_arr / g_num_threads;
    struct timeval start, end;
    double time_spent;

    std::ifstream in("gen_data.txt");

    std::string tmp;
    long int i;
    for (i = 0; i < *g_size_arr; ++i) {
        // arr[i] = std::to_string(rand() % 100000);
        getline(in, tmp);
        arr[i] = stoi(tmp);
    }
    in.close();

    pthread_t threads[g_num_threads];
    gettimeofday(&start, NULL);

    for (i = 0; i < g_num_threads; ++i) {
        struct params* p = new params;
        p->id_th = i;
        p->arr = arr;
        p->num_th = g_num_threads;
        int err = pthread_create(&threads[i], NULL, thread_merge_sort, (void *) p);
        if (err){
            printf("ERROR return code from pthread_create() is %d\n", err);
            exit(-1);
        }
    }

    for(i = 0; i < g_num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    merge_sections(arr, g_num_threads, size_sub_arr, 1);

    gettimeofday(&end, NULL);
    time_spent = ((double) ((double) (end.tv_usec - start.tv_usec) / 1000000 + (double) (end.tv_sec - start.tv_sec)));
    printf("\tTiming: %f s\n", time_spent);

    delete g_size_arr;

    // test
    test_order(arr);

    return 0;
}

void merge(long int arr[], long int left, long int middle, long int right) {
    long int k = 0;
    long int left_length = middle - left + 1;
    long int right_length = right - middle;
    long int left_array[left_length];
    long int right_array[right_length];
    long int i;
    long int j;
    for (i = 0; i < left_length; ++i)
        left_array[i] = arr[left + i];

    for (j = 0; j < right_length; ++j)
        right_array[j] = arr[middle + 1 + j];

    i = 0;
    j = 0;

    while (i < left_length && j < right_length) {
        if (left_array[i] <= right_array[j]) {
            arr[left + k] = left_array[i];
            ++i;
        } else {
            arr[left + k] = right_array[j];
            ++j;
        }
        ++k;
    }

    while (i < left_length) {
        arr[left + k] = left_array[i];
        ++k;
        ++i;
    }
    while (j < right_length) {
        arr[left + k] = right_array[j];
        ++k;
        ++j;
    }
}

void merge_sections(long int arr[], long int num_thread, long int size_sub_arr, long int aggregation) {
    long int left;
    long int right;
    long int middle;
    for(long int i = 0; i < num_thread; i = i + 2) {
        left = i * (size_sub_arr * aggregation);
        right = ((i + 2) * size_sub_arr * aggregation) - 1;
        middle = left + (size_sub_arr * aggregation) - 1;
        if (right >= *g_size_arr) {
            right = *g_size_arr - 1;
        }
        merge(arr, left, middle, right);
    }
    if (num_thread / 2 > 0) {
        merge_sections(arr, num_thread / 2, size_sub_arr, aggregation * 2);
    }
}

void merge_sort(long int arr[], long int left, long int right) {
    long int middle;
    if (left < right) {
        middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

void *thread_merge_sort(void* arg) {
    struct params * p = (struct params *)arg;
    long int thread_id = p->id_th;
    long int *arr = p->arr;
    long int num_threads = p->num_th;
    long int left = thread_id * (*g_size_arr / num_threads);
    long int right = (thread_id + 1) * (*g_size_arr / num_threads) - 1;
    if (thread_id == num_threads - 1) {
        right += (*g_size_arr % num_threads);
    }
    long int middle = left + (right - left) / 2;
    if (left < right) {
        merge_sort(arr, left, right);
        merge_sort(arr, left + 1, right);
        merge(arr, left, middle, right);
    }
    pthread_exit(nullptr);
}

void test_order(long int arr[]) {
    long int max = arr[0];
    long int i;
    for (i = 1; i < *g_size_arr; ++i) {
        if (arr[i] >= max) {
            max = arr[i];
        } else {
            printf("Error Index %ld: prev %ld > current %ld\n", i, arr[i-1], arr[i]);
            return;
        }
    }
    for (i = 1; i < *g_size_arr; ++i) {
        printf("%ld\n", arr[i]);
    }
    printf("\tArray is in sorted order\n");
}