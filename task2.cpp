#include <omp.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <limits>

using namespace std;
vector<int> matrix;

struct result {
    int max_value, min_value;
    vector<int> max_coor;
    vector<int> min_coor;
};

result answer;

result function(int threadsCount) {
    omp_set_num_threads(threadsCount);

    //array of minimum/maximum values in every thread
    vector<int> arr_min_val(threadsCount, std::numeric_limits<int>::max());
    vector<int> arr_max_val(threadsCount, std::numeric_limits<int>::min());
    //array of their coordinates
    vector<vector<int> > max_coor(threadsCount);
    vector<vector<int> > min_coor(threadsCount);
    int size(matrix.size());

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < size; ++i) {
        // get number of thread

        int threadNum(omp_get_thread_num());
        if (matrix[i] == arr_min_val[threadNum]) {
            min_coor[threadNum].push_back(i);
        }
        if (matrix[i] < arr_min_val[threadNum]) {
            arr_min_val[threadNum] = matrix[i];
            min_coor[threadNum].clear();
            min_coor[threadNum].push_back(i);
        }
        if (matrix[i] == arr_max_val[threadNum]) {
            max_coor[threadNum].push_back(i);
        }
        if (matrix[i] > arr_max_val[threadNum]) {
            arr_max_val[threadNum] = matrix[i];
            max_coor[threadNum].clear();
            max_coor[threadNum].push_back(i);
        }
    }

    answer.max_value = arr_max_val[0];
    answer.min_value = arr_min_val[0];
    answer.max_coor = max_coor[0];
    answer.min_coor = min_coor[0];

    for (int i = 0; i < threadsCount; ++i) {
        if (arr_max_val[i] > answer.max_value) {
            answer.max_value = arr_max_val[i];
            answer.max_coor.clear();
            answer.max_coor = max_coor[i];
        }
        if (arr_max_val[i] == answer.max_value) {
            answer.max_coor.insert(answer.max_coor.end(), max_coor[i].begin(), max_coor[i].end());
        }
        if (arr_min_val[i] > answer.min_value) {
            answer.min_value = arr_min_val[i];
            answer.min_coor.clear();
            answer.min_coor = min_coor[i];
        }
        if (arr_min_val[i] == answer.min_value) {
            answer.min_coor.insert(answer.min_coor.end(), min_coor[i].begin(), min_coor[i].end());
        }
    }
    return answer;
}

int main(int argc, char** argv) {
    // get parametrs - number of threads, size of matrix
    int threads_count = atoi(argv[1]);
    int matrix_size = atoi(argv[2]);
    int size = matrix_size * matrix_size;

    // write random values into matrix
    for (int i = 0; i < size; ++i)
        matrix.push_back(rand());

    if (argc < 2)
        exit(1);

    result answer;
    double begin, end, total;

    begin = omp_get_wtime();
    // algorithm starts
    answer = function(threads_count);
    end = omp_get_wtime();

    std::cout << " Time: "
        << end - begin << std::endl;

    return 0;
}
