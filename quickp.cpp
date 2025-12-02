#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

vector<int> generate_random(int n) {
    vector<int> v(n);
    for (int i = 0; i < n; i++)
        v[i] = rand();
    return v;
}

int partition_vec(vector<int> &vec, int low, int high) {
    int pivot = vec[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (vec[j] <= pivot) {
            i++;
            swap(vec[i], vec[j]);
        }
    }
    swap(vec[i + 1], vec[high]);
    return i + 1;
}

void quickSort(vector<int> &vec, int low, int high) {
    if (low >= high) return;

    int pi = partition_vec(vec, low, high);
    int len = high - low + 1;

    const int THRESH = 200000;

    if (len <= THRESH) {
        quickSort(vec, low, pi - 1);
        quickSort(vec, pi + 1, high);
    } else {
        #pragma omp task
        quickSort(vec, low, pi - 1);

        #pragma omp task
        quickSort(vec, pi + 1, high);

        #pragma omp taskwait
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    omp_set_dynamic(0);

    int n = stoi(argv[1]);
    vector<int> vec = generate_random(n);

    #pragma omp parallel
    {
        #pragma omp single
        quickSort(vec, 0, n - 1);
    }

    return 0;
}

