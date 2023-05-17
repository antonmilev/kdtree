#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <math.h>
#include "KDTree.h"

#define RMAX            10000
#define POINTS_NUM      1000000
#define TEST_NUM        100
#define KD_MAX_POINTS   2000000    //max KD tree points number  
#define SD              10			 //current space dimension

KDTree<int>  Tree(KD_MAX_POINTS, SD);

void generate_random_point(int* p, int sd)
{
	for(int k=0; k<sd; k++)
		p[k] = rand() % RMAX ;
}

int X[] = { 8177,3046,1258,2116,4818,2438,3506 };
void CreateRandomTree(int nNodes)
{
	int x[SD]; // random point
	for(int n=0; n<nNodes; n++)
	{
		generate_random_point(x, SD);
		Tree.add(x);

        if (equal(x, X, SD))
        {
            auto ccc = Tree.find_exact(X);
            int aa = 0;
        }
	}
}

void TestKDPerformance()
{
    auto timeStart = std::chrono::system_clock::now();
	CreateRandomTree(POINTS_NUM);
	auto timeEnd = std::chrono::system_clock::now();
    float kd_create_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)1000;

	printf("KD Tree Created! \n");
	printf("Points number:\t\t%d \n", POINTS_NUM);
	printf("Tree Nodes:\t\t%d\n",Tree.size());
	printf("Space Dimension:\t%d \n", SD);
	printf("Creation Time [ms]:\t%f\n", kd_create_time);
	printf("Test points:\t\t%d \n", TEST_NUM);
	
	int pTarget [SD*TEST_NUM];
    unsigned int id_kd[TEST_NUM] = { 0 }; 
    unsigned int id_brute[TEST_NUM] = { 0 };
    unsigned int n_kd_exact=0;
    unsigned int n_brute_exact=0;
	int n_errors = 0;
    float avr_kd_nn_time = 0, avr_brute_nn_time = 0, avr_kd_exact_time = 0, avr_brute_exact_time = 0;
	float avr_checked_nodes =0;

    for (int n = 0; n < TEST_NUM; n++)
    {
        generate_random_point(pTarget+SD*n, SD);
    }

    timeStart = std::chrono::system_clock::now();
    for (int n = 0; n < TEST_NUM; n++)
    {
        auto nearest = Tree.find_nearest(pTarget+SD*n);
        avr_checked_nodes += Tree.numChecked();
        id_kd[n] = nearest->getId();
    }
    timeEnd = std::chrono::system_clock::now();

    avr_kd_nn_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)(TEST_NUM*1000);
    
    timeStart = std::chrono::system_clock::now();    
    for (int n = 0; n < TEST_NUM; n++)
    {
        if(Tree.find_exact(pTarget+SD*n))
            n_kd_exact++;
    }
    timeEnd = std::chrono::system_clock::now();

    avr_kd_exact_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)(TEST_NUM*1000);

    timeStart = std::chrono::system_clock::now();
    for (int n = 0; n < TEST_NUM; n++)
    {
        auto nearest_brute = Tree.find_nearest_brute(pTarget+SD*n);
        id_brute[n] = nearest_brute->getId();
    }
	timeEnd = std::chrono::system_clock::now();	
    
    avr_brute_nn_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)(TEST_NUM*1000);
		
    timeStart = std::chrono::system_clock::now();
    for (int n = 0; n < TEST_NUM; n++)
    {
        if(Tree.find_exact_brute(pTarget+SD*n))
            n_brute_exact++;
    }
	timeEnd = std::chrono::system_clock::now();	
    avr_brute_exact_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)(TEST_NUM*1000);
    
    for (int n = 0; n < TEST_NUM; n++)
    {
        if (id_kd[n] != id_brute[n])
            n_errors++;
    }
   

	avr_checked_nodes /= TEST_NUM ;

    printf("\nNearest Neighbours Search\n");
	printf("------------------------------------------------\n");
	printf("%-#35s%f\n", "avr. kd NN time [ms]", avr_kd_nn_time );
    printf("%-#35s%f\n", "avr. kd exact time [ms]", avr_kd_exact_time );
	printf("%-#35s%f\n", "avr. brute NN time [ms]", avr_brute_nn_time );
    printf("%-#35s%f\n", "avr. brute exact time [ms]",avr_brute_exact_time );
	printf("%-#35s%f\n", "kd/brute NN ratio:",avr_brute_nn_time/avr_kd_nn_time );
    printf("%-#35s%f\n", "kd/brute exact ratio:",avr_brute_exact_time/avr_kd_exact_time );
	printf("%-#35s%d\n", "Errors:",  n_errors);

    Tree.clear();
}

int main(int argc, char* argv[])
{
    TestKDPerformance();
	return 0;
}

