#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

vector<int> generate_random(int n) {
    vector<int> v(n);
    for (int i = 0; i < n; i++)
        v[i] = rand();
    return v;
}

void merge(vector<int>& vec, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> leftVec(n1), rightVec(n2);

    for (i = 0; i < n1; i++)
        leftVec[i] = vec[left + i];
    for (j = 0; j < n2; j++)
        rightVec[j] = vec[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (leftVec[i] <= rightVec[j]) {
            vec[k] = leftVec[i];
            i++;
        } else {
            vec[k] = rightVec[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        vec[k++] = leftVec[i++];
    }
    while (j < n2) {
        vec[k++] = rightVec[j++];
    }
}

void mergeSort(vector<int>& vec, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // ---- PARALLEL REGION START ----
        #pragma omp task shared(vec) if(right - left > 1000)
        mergeSort(vec, left, mid);

        #pragma omp task shared(vec) if(right - left > 1000)
        mergeSort(vec, mid + 1, right);

        #pragma omp taskwait
        // ---- PARALLEL REGION END ----

        merge(vec, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    int n = stoi(argv[1]);
    vector<int> vec = generate_random(n);

    #pragma omp parallel
    {
        #pragma omp single
        {
            cout << "Threads in team: " << omp_get_num_threads() << "\n";
            mergeSort(vec, 0, n - 1);
        }
    }

    return 0;
}

