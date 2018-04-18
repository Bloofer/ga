#include "maxcut.h"
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

// input informations (TODO: need to be initialized)
int vertexNum = 40;
int edgeNum = 100;

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

// initial functions & functions getting input info
void read_file();
void init();
void init_popluation(); // initialize popluation to start evolution
void init_chromosome(Chromosome& ch); // initialize choromosome. used in init_population()

// function pointer
int compare_quality(const void* c1, const void* c2); // used for qsort

// GA operators
void qualify_chromosome(Chromosome &ch); // get quality of particular chromosome (=evaluate cuts)
void qualify_popluation(); // get quality of population (all chromosomes)
void selection(int &p1, int &p2); // selection of chromosome
void crossover(const Chromosome &p1, const Chromosome &p2, Chromosome &offspring); // crossover and create new chromosome from two parents
void mutation(Chromosome &ch); // mutation of chromosome
void replacement(Chromosome* offsprings); // replacement of population(?)

// time checkers (to comply time restrictions)
void check_timeout(); // check 3 minutes (proj timeout)

// main functions
void ga(); // ga 1 generation

// logging functions
void check_generation(); // check generation count
void check_operation(); // check operation count

// final report
void report_best_solution(); // reports the final best solution

// test & print functions
void print_chromosome(const Chromosome* ch);
void print_population();
void print_graph();
void test_selection();
void test_crossover();
void test_mutation();
void test_replacement();

/*
 * Function implementations
 */

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

void crossover(const Chromosome &p1, const Chromosome &p2, Chromosome &offspring)
{
    /*
     * random 1-point crossover
     */
    
    // 1. set the initial memory of the offspring
    memset(&offspring, 0x00, sizeof(offspring));

    Chromosome temp_o1, temp_o2;
    memset(&temp_o1, 0x00, sizeof(temp_o1));
    memset(&temp_o2, 0x00, sizeof(temp_o2));

    // 2. get the random point of the gene
    int random_point = rand() % (vertexNum - 1); // possible cut points are n-1

    // 3. make crossover of from parents - make two crossovers
    memcpy(temp_o1.gene, p1.gene, sizeof(bool) * vertexNum);
    memcpy(temp_o1.gene, p2.gene, sizeof(bool) * (random_point+1));

    memcpy(temp_o2.gene, p2.gene, sizeof(bool) * vertexNum);
    memcpy(temp_o2.gene, p1.gene, sizeof(bool) * (random_point+1));

    // 4. qualify two offsprings
    qualify_chromosome(temp_o1);
    qualify_chromosome(temp_o2);

    // 5. apply better one
    if(temp_o1.quality > temp_o2.quality)
        memcpy(&offspring, &temp_o1, sizeof(offspring));
    else
        memcpy(&offspring, &temp_o2, sizeof(offspring));

}

void mutation(Chromosome &ch)
{
    /* 
     * simple uniform mutation (random)
     */

    int randomProb;
    
    // generate random mutation based on mutation probability
    for(int i=0; i<vertexNum; i++)
    {
        randomProb = rand() % 1000;

        // if random probability is lower than mutation probability, flip
        if ((mutationProb*1000) >= randomProb) ch.gene[i] = !ch.gene[i];

    }

}

// used for qsort as a function pointer
int compare_quality(const void* c1, const void* c2)
{
    Chromosome *ch1 = (Chromosome *)c1;
    Chromosome *ch2 = (Chromosome *)c2;

    // compares chromosomes by quality
    if(ch1->quality > ch2->quality)
        return 1;
    else if(ch1->quality < ch2->quality)
        return -1;
    else
        return 0;

}

void replacement(Chromosome* offsprings)
{
    /*
     * genitor-style replacement (replace worst k solutions with offsprings)
     * TODO: finish this impl
     */

    // 1. sort solutions by quality
    qsort(population, n, sizeof(Chromosome), compare_quality);

    // 2. replace first k arrays with offsprings
    for(int i=0; i<k; i++)
        memcpy(&population[i], &offsprings[i], sizeof(offsprings[i]));

}

void check_timeout()
{
    // nom nom ...    
}

void report_best_solution()
{
    // nom nom ...        
}

void qualify_chromosome(Chromosome &ch)
{
    ch.quality = 0;

    int toVertex, fromVertex;

    // iterate all edges in the graph
    for(int i=0; i<edgeNum; i++)
    {
        toVertex = edges[i].to;
        fromVertex = edges[i].from;

        // add weight of the edge if two vertices are inverted
        if (ch.gene[toVertex-1] != ch.gene[fromVertex-1]) ch.quality += edges[i].weight;

    }
}

void init_chromosome(Chromosome& ch)
{
    for(int i=0; i<edgeNum; i++){
        ch.gene[i] = rand()%2;
    }
    qualify_chromosome(ch);
}

// initial function for creating new population
void init_popluation()
{
    // creates population(solution set) by random
    for(int i=0; i<n; i++){
        init_chromosome(population[i]);
    }
}

void ga()
{

    bool stopCondition = false;

    do {

        Chromosome offsprings[k];
        memset( offsprings, 0, sizeof(Chromosome)*k ); // initialize the size of offsprings

        // iterate ga operators for k times
        for (int i=0; i<k; i++){
            int p1, p2;
            selection(p1, p2);
            crossover(population[p1],population[p2],offsprings[i]);
            mutation(offsprings[i]);
        }
        replacement(offsprings);

        // TODO: stopCondition must be tuned in this loop
        check_timeout();

    } while (!stopCondition);

    report_best_solution();

}



/*
 * TEST FUNCTIONS
 */

void print_chromosome(const Chromosome& ch)
{

    printf("========== print chromosome ==========\n");
    printf("genes : ");
    for(int i=0; i<vertexNum; i++){
        printf("%d ", ch.gene[i]);
    }
    printf("\nquality : %d\n", ch.quality);
    printf("======================================\n\n");

}

void print_population()
{

    printf("Number of population: %d\n\n",n);
    for(int i=0; i<n; i++){
        print_chromosome(population[i]);
    }

}

void print_graph()
{

    printf("========== print graph data ==========\n");
    printf("Vertex#: %d\tEdge#:%d\n",vertexNum,edgeNum);
    for(int i=0; i<edgeNum; i++){
        printf("%d\t%d\t%d\n",edges[i].from,edges[i].to,edges[i].weight);
    }
    printf("======================================\n\n");

}

void test_selection(){

    int p1, p2;
    printf("Test selection...\n");
    for(int i=0; i<20; i++){
        selection(p1,p2);
        printf("%d, %d\n",p1,p2);
    }

}

void test_crossover(){

    Chromosome test_ch1, test_ch2, test_ch3;
    for(int i=0; i<vertexNum; i++){
        // test case 000000/111111...
        test_ch1.gene[i] = 0;
        test_ch2.gene[i] = 1;
    }

    printf("<Before crossover>\n");
    qualify_chromosome(test_ch1);
    print_chromosome(test_ch1);
    qualify_chromosome(test_ch2);
    print_chromosome(test_ch2);
    
    crossover(test_ch1, test_ch2, test_ch3);
    qualify_chromosome(test_ch3);
    print_chromosome(test_ch3);

}

void test_mutation(){

    Chromosome test_ch1;
    for(int i=0; i<vertexNum; i++){
        // test case 000000...
        test_ch1.gene[i] = 0;
    }

    mutation(test_ch1);
    print_chromosome(test_ch1);

}

void test_replacement(){

    // need to make graph file reader first...

}



/*
 * Initial functions & File read function
 */

// reads file and renders to graph data structure
void read_file(char* fileName){

    ifstream gfile(fileName);
    string firstLine; // string variable to parse first line of the file
    //getline(gfile, firstLine);
    gfile >> vertexNum >> edgeNum;

    int from, to, weight;
    int edgeIndex = 0;
    while (gfile >> from >> to >> weight){
        edges[edgeIndex].from = from;
        edges[edgeIndex].to = to;
        edges[edgeIndex].weight = weight;
        edgeIndex++;
    }

}

void init()
{
    // TODO: reset mem of the edges[] & Chromosome.gene
    // is it OK to give longer size of the array and use less?

    // marks start time of the program
    clock_t startTime = clock();

    // sets random timer to srand()
    srand(time(NULL));

    init_popluation();
    print_population(); // just for test.

}


// just for tests. need to be replaced
int main(int argc, char** argv)
{

    // TODO: this should be put on init function
/* 
    while(true){

        clock_t currentTime = clock();

        // do nothing ...

        float timeDuration = (currentTime - startTime) / CLOCKS_PER_SEC;

        printf("Time duration : %f\n", timeDuration);    

        if (timeDuration > 5.0) {
            printf("Halt program! Time duration : %f", timeDuration);    
            return 0;
        }

    } */

    /*
     * File read test
     */

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " GRAPHFILE" << endl;
        return 1;
    }

    read_file(argv[1]);
    init();

    //print_graph();
    //test_selection();
    //test_crossover();
    //test_mutation();


    return 0;

}
