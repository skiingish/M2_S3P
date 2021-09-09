#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>

using namespace std::chrono;
using namespace std;

const int NUM_THREADS = 12;

void randomVector(int vector[], int size)
{
    //Begin parallel segment
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            //assign random number to elements
            vector[i] = rand() % 100;
        }

    }
 
}


int main(){

    unsigned long size = 10000000;
    unsigned long total = 0;

    srand(time(0));

    int *v1, *v2, *v3;


    //Start clock
    auto start = high_resolution_clock::now();

    
    v1 = (int *) malloc(size * sizeof(int *));
    v2 = (int *) malloc(size * sizeof(int *));
    v3 = (int *) malloc(size * sizeof(int *));

    omp_set_num_threads(NUM_THREADS);


    randomVector(v1, size);

    randomVector(v2, size);

    
    //Sum all elements in v3
    #pragma omp parallel default(none) firstprivate(v1, v2, v3, size) 
    {
        #pragma omp for schedule(dynamic, 10)
        for (int i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }
        size = 20;
    }
    cout << size << endl;

    auto startatomic = high_resolution_clock::now();
    #pragma omp parallel
    {
        unsigned long localsum = 0;

        #pragma omp for
        for(int i = 0; i < size; i++)
        {
            localsum += v3[i];
        }
        #pragma omp critical
        total += localsum;
    }

    unsigned long total1 = 0;
    auto startreduction = high_resolution_clock::now();
    #pragma omp parallel reduction(+:total1)
    {
        #pragma omp for
        for(int i = 0; i < size; i++)
        {
            total1 += v3[i];
        }
    }

    auto stop = high_resolution_clock::now();

    
    auto duration = duration_cast<microseconds>(stop - start);
    auto durationatomic = duration_cast<microseconds>(startreduction - startatomic);
    auto durationreduction = duration_cast<microseconds>(stop - startreduction);


    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl
         << durationatomic.count() << endl << durationreduction.count() << endl
         << total << endl
         << total1 << endl;

    return 0;
}