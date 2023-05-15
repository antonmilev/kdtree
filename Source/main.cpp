#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <math.h>
#include "KDTree.h"

#define MAX_DIM  20             //max space dimension
#define RMAX 10000
#define POINTS_NUM 1000000
#define TEST_NUM 50

KDTree<int>  Tree;

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
	printf("Tree Nodes:\t\t%d\n",Tree.nList);
	printf("Space Dimension:\t%d \n", SD);
	printf("Creation Time [ms]:\t%f \n", kd_create_time);
	printf("\nTest points:\t\t%d \n", TEST_NUM);
	
	int pTarget [SD];
	int n_errors = 0;
	float dis_kd, dis_brute ;
	float kd_time, brute_time ;
	float avr_kd_time = 0;
	float avr_brute_time = 0;
	float avr_checked_nodes =0;
	int internal_error = 0;
	float avr_dis_kd=0 ;
	float avr_dis_brute=0 ; 

    for (int nTest = 0; nTest < TEST_NUM; nTest++)
    {
        generate_random_point(pTarget, SD);

        timeStart = std::chrono::system_clock::now();
        auto nearest = Tree.find_nearest(pTarget);
        timeEnd = std::chrono::system_clock::now();

        kd_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)1000;

		dis_kd = (float)sqrt(Tree.d_min) ;

        timeStart = std::chrono::system_clock::now();
		auto nearest_brute = Tree.find_nearest_brute(pTarget);
		timeEnd = std::chrono::system_clock::now();	

		brute_time = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / (float)1000;
		dis_brute = (float)sqrt(Tree.d_min) ;

		if(!nearest || !nearest_brute)
		{
			internal_error++ ;
		}

        if (!Tree.find_exact(nearest->x))
        {
            internal_error++ ;
        }

		if(nearest && nearest_brute)
		{			
			if(nearest_brute->id != nearest->id && fabs(dis_kd - dis_brute) > 0.001)
			{
				printf("\n-------------------------------------------\n");
				printf("\nKD Find Nearest Result: \n");
				if(!nearest)
				{
					printf("Search Failed! The tree is empty!");
				}
				else
				{
					printf("Point ID:\t\t%d\n",  nearest->id );
//					printf("Parent ID:\t\t%d\n",  Tree.nearest_parent->id );		
					printf("distance:\t\t%f\n",  dis_kd);
					printf("checked nodes:\t\t%d\n",  Tree.checked_nodes );
					printf("ålapsed time [ms]\t%f\n", kd_time);
				}
				printf("\nBrute Force Result : \n");	

				if(!nearest_brute)
				{
					printf("Search Failed! The tree is empty!");
				}
				else
				{
					printf("Point ID:\t\t%d\n",  nearest_brute->id );
					printf("distance:\t\t%f\n",  dis_brute);
					printf("ålapsed time [ms]\t%f",brute_time );
				}
				n_errors++;
			}

			avr_checked_nodes+= Tree.checked_nodes;
			avr_brute_time += brute_time ;
			avr_kd_time += kd_time ;
			avr_dis_kd += dis_kd ;
		}
	}

	avr_brute_time /= TEST_NUM;
	avr_kd_time /= TEST_NUM;
	avr_checked_nodes /= TEST_NUM ;
	avr_dis_kd /= TEST_NUM;

	printf("\n----------------------------------\n");
	printf("avr. kd time [ms]\t%f\n",avr_kd_time );
	printf("avr. check nodes\t%0.2f\n",avr_checked_nodes );
	printf("avr. brute time [ms]\t%f\n",avr_brute_time );
	printf("kd/brute ratio\t\t%f\n",avr_brute_time/avr_kd_time );
	printf("Errors:\t\t\t%d\n",  n_errors);
	printf("Internal Errors:\t%d\n",  internal_error);

    Tree.clear();
}

int main(int argc, char* argv[])
{
    TestKDPerformance();

	return 0;
}

