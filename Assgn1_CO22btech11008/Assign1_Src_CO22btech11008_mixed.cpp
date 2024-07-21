#include <iostream>
#include <pthread.h>
#include <fstream>
#include <chrono>
#define Max 10000
int N;
int K;
int A[Max][Max];
int ans[Max][Max];
float p;

using namespace std;

// Function to perform matrix multiplication for a specific row
void *Matrix_multiplication(void *arg)
{
    int num = *((int *)arg);

    if (num < K - 1)
    {
        int m = num;
        while (m < N)
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
            m = m + K;
        }
    }
    else
    {
        int m = num;
        while (m < N)
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
            m = m + K;
        }
    }

    pthread_exit(NULL);
}

int main()
{
    /******************inputs**********/

    ifstream inputFile("inp.txt");
    // inputFile >> N;
    // inputFile >> K;
    N = 2048;
    K = 16;
    int w = 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // inputFile >> A[i][j];
            A[i][j] = w;
            w++;
            ans[i][j] = 0;
            // w++;
        }
    }
    // N =16;

    /*********FILE generation*************/

    ofstream output("output_mixed.txt");
    // ofstream k_VS_time("mixed_method_k_vs_timing.txt");
    // ofstream N_vs_time("mixed_method_time_vs_N.txt");
    // K=8;
    // for (int l = 0; l < 8; l++)
    {

        p = (N / K);
        
        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        int numbers[K + 1];
        for (int i = 0; i < K; i++)
        {
            numbers[i] = i;
        }
        p = N / K;
        p=16;
        pthread_t threads[K];

        /******************************* threads creation***********************************************************/

        // Creating threads and pass A matrix, ans matrix, and row index as arguments
        for (int i = 0; i < K; i++)
        {

            int rc = pthread_create(&threads[i], NULL, Matrix_multiplication, (void *)(&numbers[i]));
            if (rc)
            {
                cerr << "Error creating thread, return code: " << rc << endl;
                return -1;
            }
        }

        // Waiting for threads to finish
        for (int i = 0; i < K; i++)
        {
            pthread_join(threads[i], NULL);
        }

        // Display the resulting ans matrix
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                output << ans[i][j] << ' ';
            }
            output << '\n';
        }

        /********** for analysis perpose *****************************/

        // k_VS_time << K << " " << ((double)(end - start)) / CLOCKS_PER_SEC << endl;
        // output << "N=" << N << " K=" << K << " time =" << ((double)(end - start)) / CLOCKS_PER_SEC << endl;
        // N_vs_time << N << " " << ((double)(end - start)) / CLOCKS_PER_SEC << endl;
        // N=N*2;
        // K = K * 2;
        chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end - start);


        cout << "K=" << K << " Duration=" << duration.count() << " seconds\n";
        // k_VS_time << K << " " << duration.count();
        // N_vs_time << N << " " << duration.count();
        // output <<"N="<<N<<"K="<< K<<"time ="<<duration.count();
        // k_VS_time <<  "\n" ;
        // N_vs_time << "\n " ;  
        // N=N*2      

        // K = K * 2;  
          }

    return 0;
}
