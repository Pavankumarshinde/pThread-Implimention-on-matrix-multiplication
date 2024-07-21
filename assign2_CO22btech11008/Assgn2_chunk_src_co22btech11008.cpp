#include <iostream>
#include <pthread.h>
#include <fstream>
#include <chrono> 
#define Max 10000

int N;
int K;
int C;
int A[Max][Max];
int ans[Max][Max];
float p;
int BT;
float bounded;
float unbounded;

using namespace std;

void *Matrix_multiplication(void *arg)
{
    
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    
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

        chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end - start);
        if(num<BT)//bounded. else unbounded.
    {
        // cout<<"thread time:(bounded)"<<duration.count()<<endl;
        bounded+=duration.count();
        // cout<<"bounded:"<<bounded;

    }
    else{
        //  cout<<"thread time (unbounded):"<<duration.count()<<endl;
        unbounded+=duration.count();
        // cout<<"unbounded:"<<unbounded;


    }
    // cout<<endl;

    pthread_exit(NULL);
}

int main()
{
    N = 1024;
    K = 4;
    C = 16;

    C=C/2;// as in experiment to we want oassign only to C/2 cores.

    // int b=K/C; 
    //  BT=b
    
    // BT=0;


    int w = 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = w;
            w++;
            ans[i][j] = 0;
        }
    }

    ofstream output("output_chunk_method.txt");
    ofstream BT_vs_time("chunk_method_BT_vs_time.txt",ios::app);
    ofstream K_vs_time("chunk_method_K_vs_time.txt",ios::app);

    
    // while(BT<K)
    while(K<=64)
    {
        bounded=0;
        unbounded=0;
         BT=K/2;
        // BT=0;
        p = float(N / K);

        int numbers[K + 1];
        for (int i = 0; i < K; i++)
        {
            numbers[i] = i;
        }
        p = N / K;

        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        pthread_t threads[K];
       
        
        for (int i = 0; i < K; i++)
        {
            int rc = pthread_create(&threads[i], NULL, Matrix_multiplication, (void *)(&numbers[i]));
            if (rc)
            {
                cerr << "Error creating thread, return code: " << rc << endl;
                return -1;
            }

            if (i < BT)
            {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                int core_to_bind = i / C;
                CPU_SET(core_to_bind, &cpuset);
                pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
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

        // cout << "K=" << K << " Duration=" << duration.count() << " seconds\n";
        BT_vs_time << BT << " " << duration.count() << "\n";
        K_vs_time << K << " " << duration.count() << "\n";


        
        // BT=b+BT;
        printf("K=%d  average_bounded:%f\n",K,2*bounded/K);
        // printf("K=%d  average_unbounded:%f\n",K,2*unbounded/K);
        K = K * 2;
    }

    return 0;
}
