#include <iostream>
#include <pthread.h>
#include <string>
#include <cmath>

using namespace std;


int g_size_arr;
int g_num_threads;
pthread_mutex_t g_mutex;

struct params {
    string* array;
    int left;
    int right;
    string* mod;
};

int char_to_int(char c) {
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
    int vec_a = 0;
    int vec_b = 0;
    int k = 1;
    int i;
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

void merge(string *array, int left, int middle, int right, string *modif) {
    int l = left;
    int r = middle;
    for (int i = left; i < right; ++i)
        if (l < middle && (r >= right || a_lower_or_eq_b(array[l], array[r])))
            modif[i] = array[l++];
        else
            modif[i] = array[r++];
    for (int i = left; i < right; ++i)
        array[i] = modif[i];
}

void* split(void* param) {
    struct params* temp_args = new params;
    temp_args = (params*)param;

    if (temp_args->right - temp_args->left < 2)
        return NULL;

    int tmp_right = temp_args->right;
    int tmp_left = temp_args->left;

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

    int new_left;
    int new_right;
    for (int i = 0; i < g_num_threads; ++i) {
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

    int j;
    int left;
    int right;
    for (int i = g_num_threads / 2; i > 0; i = i >> 1) { //divide by 2
        for (j = 0; j < i; ++j) {
            left = j * g_size_arr / i;
            right = (j + 1) * g_size_arr / i;

            merge(array, left, (left + right) / 2, right, tmp);
        }
    }
}

int main(int argc, char *argv[]) {
    cin >> g_size_arr;

    string array[g_size_arr];

    for (int i = 0; i < g_size_arr; ++i)
        cin >> array[i];
        // array[i] = (int)(rand() % 100);

    g_num_threads = atoi(argv[1]);

    int power = 0;
    while (g_num_threads > 0) {
        g_num_threads = g_num_threads >> 1;  // div 2
        ++power;
    }

    --power;
    if (!power)
        power = 1;

    g_num_threads = (int)pow(2.0, (double)(power));

    merge_sort(array);

    cout << "\n Sorted array:\t";
    for (int i = 0; i < g_size_arr; ++i)
        cout << array[i] << " ";
    cout << "\n";
    return 0;
}