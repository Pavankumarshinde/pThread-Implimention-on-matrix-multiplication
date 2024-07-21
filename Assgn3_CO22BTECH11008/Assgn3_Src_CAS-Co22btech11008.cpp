#include <iostream>
#include <pthread.h>
#include <atomic>
#include <fstream>
#include <chrono>
#define Max 10000

int N;
int K;
int C = 0;
int rowInc;

int A[Max][Max];
int ans[Max][Max];

using namespace std;

std::atomic<bool> lock(false);

void critical_section(int &start, int &end)
{
    start = C;
    C += rowInc;
    end = C;
}

void thread_function(int &start, int &end)
{
    while (true)
    {
        bool expected = false;
        if (lock.compare_exchange_weak(expected, true, std::memory_order_acquire))
        {
            critical_section(start, end);

            lock.store(false, std::memory_order_release);
            break;
        }
    }
}

void *Matrix_multiplication(void *arg)
{
    int start = -1;
    int end = -1;
    thread_function(start, end);

    if (K * rowInc > end)
    {
        for (int m = start; m < end && m < N; m++)
        {
            for (int n = 0; n < N; n++)
            {
                int sum = 0;
                for (int i = 0; i < N; i++)
                {
                    sum += A[m][i] * A[i][n];
                }
                ans[m][n] = sum;
            }
        }
    }

    else
    {
        for (int m = start; m < N; m++)
        {
            for (int n = 0; n < N; n++)
            {
                int sum = 0;
                for (int i = 0; i < N; i++)
                {
                    sum += A[m][i] * A[i][n];
                }
                ans[m][n] = sum;
            }
        }
    }

    pthread_exit(NULL);
}

int main()
{
    // ifstream inputFile("inp.txt");
    // inputFile >> N;
    // inputFile >> K;
    // inputFile>> rowInc;
    N = 2048;
    K = 16;
    rowInc = 16;

    ofstream output("output_chunk_method.txt");
    int w = 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // inputFile>>A[i][j];
            A[i][j] = w;

            w++;
            ans[i][j] = 0;
        }
    }

    // while(K<=32)
    // while(N<=4096)
    // while(rowInc<=32)
    {
        C = 0;

        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        pthread_t threads[K];

        for (int i = 0; i < K; i++)
        {
            int rc = pthread_create(&threads[i], NULL, Matrix_multiplication, (void *)(&C));
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

        output << "N=" << N << "K=" << K << "time =" << duration.count();
        cout << K << " " << duration.count() << endl;

        // N=N*2;
        // rowInc=rowInc*2;
        // K=K*2;
    }

    return 0;
}
