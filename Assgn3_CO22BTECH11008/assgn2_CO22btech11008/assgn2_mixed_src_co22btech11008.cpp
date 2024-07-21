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
       chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = chrono::duration_cast<chrono::duration<double>>(end - start);

        if(num<BT)//bounded. else unbounded.
    {
        // cout<<"thread time:(bounded)"<<duration.count()<<endl;
        bounded+=duration.count();
        

    }
    else{
        //  cout<<"thread time (unbounded):"<<duration.count()<<endl;
        unbounded+=duration.count();
       


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
    N = 1024;
    K = 4;
    int C = 16;
    // int b=K/C; // 6 cores in my laptop.
    BT=0;
    // int BT=K/2;
    // C=C/2;


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
   

   

    ofstream output("output_mixed.txt");
    ofstream k_VS_time("mixed_method_k_vs_timing.txt",ios::app);
    ofstream BT_vs_time("mixed_method_time_vs_BT.txt",ios::app);



    // while(BT<K)//for experiment 1
    while(K<=64)// for experiment 2
    {
         bounded=0;
        unbounded=0;
        BT=K/2;

        p = (N / K);
        chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();

        int numbers[K + 1];
        for (int i = 0; i < K; i++)
        {
            numbers[i] = i;
        }
        p = N / K;
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


        k_VS_time << K << " " << duration.count();
        output <<"N="<<K<<"K="<< K<<"time ="<<duration.count();
        k_VS_time <<  "\n" ;
        
        // cout << "BT="<< BT << " Duration=" << duration.count() << " seconds\n";
        BT_vs_time << BT << " " << duration.count() << "\n";


        printf("K=%d  average_bounded:%f\n",K,2*bounded/K);
        // printf("K=%d  average_unbounded:%f\n",K,2*unbounded/K);


        K = K * 2;
        // BT=b+BT;
        
        
        }

    return 0;
}
