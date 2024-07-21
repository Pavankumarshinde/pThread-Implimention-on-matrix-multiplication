#include <iostream>
#include <pthread.h>
#include <atomic>
#include <fstream>
#include <chrono>
#define Max 10000

int N;
int K;
// int C=0;
std::atomic<int> C(0);
int rowInc;

int A[Max][Max];
int ans[Max][Max];

using namespace std;

void *Matrix_multiplication(void *arg)
{
    int start = C.fetch_add(rowInc);
    int end = start + rowInc;

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
    // inputFile >> C;
    // inputFile >> BT;
    N = 2048;
    K = 2;
    rowInc = 16;

    ofstream output("output.txt");
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
    while (K <= 32)
    // while(N<=4096)
    // while(rowInc<=32)
    {
        C = 0;

        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        pthread_t threads[K];

        for (int i = 0; i < K; i++)
        {
            int rc = pthread_create(&threads[i], NULL, Matrix_multiplication, (void *)(&i));
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
        K = K * 2;
    }

    return 0;
}
