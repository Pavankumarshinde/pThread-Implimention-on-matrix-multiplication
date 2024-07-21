#include <iostream>
#include <pthread.h>
#include <fstream>
#include <chrono> 
#define Max 1000
int C;
int N;
int K;
int b;
int BT;
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
        for (int m = (p * num); m < (p * (num + 1)); m++)
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
        for (int m = (p * num); m < N; m++)
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
    ifstream inputFile("inp.txt");
    inputFile >> N;
    inputFile >> K;
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
        }
    }

    ofstream output("output_chunk_method.txt");
    
    // for (int l = 0; l < 3; l++) 
    {
        // BT=BT+1;
        p = float(N / K);

        int numbers[K + 1];
        for (int i = 0; i < K; i++)
        {
            numbers[i] = i;
        }
        // p = N / K;
        p=16;

        // Use chrono.h for accurate time measurements
        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        pthread_t threads[K];
        

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

        // chrono::Stop();

        // Display the resulting ans matrix
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
        // k_VS_time << K << " " << duration.count();
        // BT_vs_time << BT << " " << duration.count();
        // output <<"N="<<N<<"K="<< K<<"time ="<<duration.count();
        // k_VS_time <<  "\n" ;
        // BT_vs_time << "\n " ;  
        // N=N*2      

        // K = K * 2;
    }

    return 0;
}
