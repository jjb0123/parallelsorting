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


void count_sort(int arr[], int n, int pos)
{
    int count[10] = {0};

    // Parallel counting
    #pragma omp parallel
    {
        int local_count[10] = {0};

        #pragma omp for nowait
        for (int i = 0; i < n; i++)
            local_count[(arr[i] / pos) % 10]++;

        #pragma omp critical
        {
            for (int i = 0; i < 10; i++)
                count[i] += local_count[i];
        }
    }

    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    //scatter
    vector<int> output(n);

    #pragma omp parallel for
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / pos) % 10;

        int idx;
        #pragma omp atomic capture
        idx = --count[digit];

        output[idx] = arr[i];
    }

    // Copy back
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        arr[i] = output[i];
}

void radix_sort(int arr[], int n)
{
    int max_val = *max_element(arr, arr + n);

    for (int pos = 1; max_val / pos > 0; pos *= 10)
        count_sort(arr, n, pos);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <size>\n";
        return 1;
    }

    int n = stoi(argv[1]);
    vector<int> vec = generate_random(n);

    int* arr = vec.data();
    radix_sort(arr, n);

    return 0;
}


