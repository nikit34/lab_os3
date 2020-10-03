#include <iostream>
#include <pthread.h>
#include <string>
#include <cmath>
#include <sys/time.h>

using namespace std;


u_long g_size_arr;
u_long g_num_threads;
pthread_mutex_t g_mutex;

struct params {
    string* array;
    u_long left;
    u_long right;
    string* mod;
};

u_long char_to_int(char c) {
    if (c >= '0' && c <= '9') {
        c -= '0';
    } else if (c >= 'p' && c <= 'z') {
        c -= 'W';
    } else if (c >= 'A' && c <= 'Z') {
        c = tolower(c);
        c -= 'W';
    }
    return c;
}

bool a_lower_or_eq_b(string a, string b) {
    u_long vec_a = 0;
    u_long vec_b = 0;
    u_long k = 1;
    u_long i;
    for(i = a.size(); i > 0; --i) {
        vec_a += char_to_int(a[i]) * k;
        k *= 10;
    }
    k = 1;
    for(i = b.size(); i > 0; --i) {
        vec_b += char_to_int(b[i]) * k;
        k *= 10;
    }
    if (vec_a <= vec_b) {
        return  true;
    }
    return false;
}

void merge(string *array, u_long left, u_long middle, u_long right, string *modif) {
    u_long l = left;
    u_long r = middle;
    for (u_long i = left; i < right; ++i)
        if (l < middle && (r >= right || a_lower_or_eq_b(array[l], array[r])))
            modif[i] = array[l++];
        else
            modif[i] = array[r++];
    for (u_long i = left; i < right; ++i)
        array[i] = modif[i];
}

void* split(void* param) {
    struct params* temp_args = new params;
    temp_args = (params*)param;

    if (temp_args->right - temp_args->left < 2)
        return NULL;

    u_long tmp_right = temp_args->right;
    u_long tmp_left = temp_args->left;

    temp_args->right = (tmp_left + tmp_right) / 2;
    split((void*)temp_args);

    temp_args->right = tmp_right;
    temp_args->left = (tmp_left + tmp_right) / 2;
    split((void*)temp_args);

    temp_args->left = tmp_left;
    merge(temp_args->array, temp_args->left, (temp_args->left + temp_args->right) / 2, temp_args->right, temp_args->mod);
    return 0;
}

void merge_sort(string *array) {
    struct params* p = new params;

    string tmp[g_size_arr];
    pthread_t threads[g_num_threads];

    u_long new_left;
    u_long new_right;
    for (u_long i = 0; i < g_num_threads; ++i) {
        new_left = i * g_size_arr / g_num_threads;
        new_right = (i + 1) * g_size_arr / g_num_threads;

        p->mod = tmp;
        p->array = array;
        p->left = new_left;
        p->right = new_right;

        pthread_mutex_init(&g_mutex, NULL);

        pthread_create(&threads[i], NULL, split, (void*)p);

        pthread_join(threads[i], NULL);

        pthread_mutex_destroy(&g_mutex);
    }

    u_long j;
    u_long left;
    u_long right;
    for (u_long i = g_num_threads / 2; i > 0; i = i >> 1) { //divide by 2
        for (j = 0; j < i; ++j) {
            left = j * g_size_arr / i;
            right = (j + 1) * g_size_arr / i;

            merge(array, left, (left + right) / 2, right, tmp);
        }
    }
}


int main(int argc, char *argv[]) {
    g_num_threads = atoi(argv[1]);

    cout << "\n\tcount elements:\t";
    // cin >> g_size_arr;
    g_size_arr = 1000000000;
    string array[g_size_arr];

    for (u_long i = 0; i < g_size_arr; ++i) {
        // cin >> array[i];
        array[i] = to_string(rand() % 100000);
    }

    // <<<< time benchmarking
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    double time_begin = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000;
    // >>>> start

    u_long power = 0;
    while (g_num_threads > 0) {
        g_num_threads = g_num_threads >> 1;  // div 2
        ++power;
    }

    --power;
    if (!power)
        power = 1;

    g_num_threads = (u_long)pow(2.0, (double)(power));


    merge_sort(array);

    cout << "\n Sorted array:\t";
    for (u_long i = 0; i < g_size_arr; ++i)
        cout << array[i] << " ";

    cout << "\n";

    // <<<< end
    gettimeofday(&tv, NULL);
    double time_end = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000 ;
    double total_time_ms = time_end - time_begin;
    cout << "\n\tTOTAL TIME:\t" << total_time_ms << " ms\n";

    return 0;
}