#include "maxcut.h"

//#include <iostream>
//#include <stdlib.h>     /* srand, rand */
//#include <time.h>       /* time */

using namespace std;

/*
 * GA variable definitions
 */

// GA variables
const int maxVertexNum = 500;
const int maxEdgeNum = 5000;
const int timeConstraint = 180;

const int n = 20; // size of population (need tuning?)
const int k = 4;  // size of replacement (need tuning?)

// input informations (need to be initialized)
int vertexNum = 0;
int edgeNum = 0;

// probabilities of mutation
float mutationProb = 0.015; // or 0.01? depends on the result

// graph data holding weight info
typedef struct{
    int from, to, weight;
}Edge;
Edge edges[maxEdgeNum];

// chromosome & population design
typedef struct{
    bool gene[maxVertexNum]; // true - S / false - S'
    int quality; // total weight of the cuts
}Chromosome;
Chromosome population[n]; // n varies on the result



/*
 * Function definitions
 */

// functions getting input info
void read_file();
void get_graph_data();

// main functions
void init_popluation(); // initialize popluation to start evolution
void evolve(); // evolve 1 generation

// GA operators
void qualify_chromosome(); // get quality of particular chromosome (=evaluate cuts)
void qualify_popluation(); // get quality of population (all chromosomes)
void selection(int &p1, int &p2); // selection of chromosome
void crossover(); // crossover and create new chromosome from two parents
void mutation(); // mutation of chromosome
void replacement(); // replacement of population(?)

// time checkers (to comply time restrictions)
void check_timeout(); // check 3 minutes (proj timeout)

// logging functions
void check_generation(); // check generation count
void check_operation(); // check operation count

// final report
void report_best_solution(); // reports the final best solution



/*
 * Function implementations
 */

void evolve()
{

    bool stopCondition = false;

    do {

        Chromosome offsprings[k];
        memset( offsprings, 0, sizeof(Chromosome)*k ); // initialize the size of offsprings

        // iterate ga operators for k times
        for (int i=0; i<k; i++){
            int dummy_p1, dummy_p2; // TODO: should replace with real p1,p2
            selection(dummy_p1, dummy_p2);
            crossover();
            mutation();
        }
        replacement();

        // TODO: stopCondition must be tuned in this loop
        check_timeout();

    } while (!stopCondition);

    report_best_solution();

}

void selection(int &p1, int &p2)
{
    /*
     * roulette wheel algorithm?
     */

    /*
     * random picking of two parents
    */

    int temp_p1, temp_p2;

    temp_p1 = rand() % n; // get random index of population to select parent
    do{
        temp_p2 = rand() % n; // p1,p2 must be different
    }while(temp_p1==temp_p2);

    p1 = temp_p1;
    p2 = temp_p2;

    // TODO: note; maybe need to change selection algorithm to random
}

void crossover()
{
    // nom nom ...
}

void mutation()
{
    // nom nom ...
}

void replacement()
{
    // nom nom ...    
}

void check_timeout()
{
    // nom nom ...    
}

void report_best_solution()
{
    // nom nom ...        
}

// just for tests. need to be replaced
int main(int argc, char** argv)
{

    // random time setter. this should be put on init function
    srand(time(NULL));
    int rand_num = rand();

    cout << rand_num << endl;

    return 0;

}
