#include <iostream>
#include <pthread.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#define Max 10000

int N;
int K;
int A[Max][Max];
int ans[Max][Max];
int SIZE;
int N_array[Max];
float p;

using namespace std;

void *Matrix_multiplication(void *arg)
{
    int num = *((int *)arg);

    if (num < K - 1)
    {
        for (int m = (p * num); m < (p * (num + 1)); m++)
        {
            int t = N_array[m];
            for (int n = 0; n < N; n++)
            {
                int sum = 0;
                for (int i = 0; i < N; i++)
                {
                    sum += A[t][i] * A[i][n];
                }
                ans[t][n] = sum;
            }
        }
    }
    else
    {
        for (int m = (p * num); m < N; m++)
        {
            int t = N_array[m];
            for (int n = 0; n < N; n++)
            {
                int sum = 0;
                for (int i = 0; i < N; i++)
                {
                    sum += A[t][i] * A[i][n];
                }
                ans[t][n] = sum;
            }
        }
    }
    pthread_exit(NULL);
}

int main()
{
    ifstream inputFile("inp.txt");
    inputFile >> N;
    inputFile >> K;
    N = 1024;
    K = 1;
    int w = 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = w;
            // inputFile>>A[i][j];
            ans[i][j] = 0;
            w++;
        }
    }
    SIZE = N;

    for (int i = 0; i < N; i++)
    {
        N_array[i] = i;
    }

    ofstream output("output_extramethod.txt");
    ofstream k_VS_time("extra_method_k_vs_timing.txt");
    ofstream N_vs_time("extra_method_time_vs_N.txt");

    for (int l = 0; l < 8; l++)
    {
        p = float(N) / float(K);

        random_shuffle(N_array, N_array + N);

        // clock_t start = clock();

        pthread_t threads[K];
        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        int numbers[K];
        for (int i = 0; i < K; i++)
        {
            numbers[i] = i;
        }

        for (int i = 0; i < K; i++)
        {
            int rc = pthread_create(&threads[i], NULL, Matrix_multiplication, (void *)(&numbers[i]));
            if (rc)
            {
                cerr << "Error creating thread, return code: " << rc << endl;
                return -1;
            }
        }

        for (int i = 0; i < K; i++)
        {
            pthread_join(threads[i], NULL);
        }

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                output << ans[i][j] << ' ';
            }
            output << '\n';
        }

        chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end - start);

        
        cout << "K=" << K << " Duration=" << duration.count() << " seconds\n";
        k_VS_time << K << " " << duration.count();
        N_vs_time << N << " " << duration.count();
        output <<"N="<<N<<"K="<< K<<"time ="<<duration.count();
        k_VS_time <<  "\n" ;
        N_vs_time << "\n " ;  
        // N=N*2      

        
        K = K * 2;
    }

    return 0;
}
