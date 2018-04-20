#include "maxcut.h"
using namespace std;

/*
 * GA variable definitions
 */

// GA variables
const int maxVertexNum = 500;
const int maxEdgeNum = 5000;
const int timeConstraint = 175.0;

const int n = 20; // size of population (need tuning?)
const int k = 4;  // size of replacement (need tuning?)

// input informations. inititalized on read_file() 
int vertexNum;
int edgeNum;

// probabilities of mutation
float mutationProb = 0.015; // or 0.01? depends on the result

// ga quality logging
int generation_count;

// fitness array used for roulette wheel algorithm
float fitness[n];

// timer variables
clock_t startTime;
clock_t currentTime;

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

// GA options to select operator strategy
typedef enum{
    RANDOM, // random pick of two parents
    ROULETTE_WHEEL // selection based on fitness function
} Selection_strategy;
Selection_strategy selectionType = ROULETTE_WHEEL;

typedef enum{
    ONE_POINT, // random one point crossover
    TWO_POINT // random two point crossover
} Crossover_strategy;
Crossover_strategy crossoverType = ONE_POINT;

/* // TODO: maybe remove this?
typedef enum{
    UNIFORM,
    NON_UNIFORM
} Mutation_strategy;
Mutation_strategy mutationType = UNIFORM;

typedef enum{
    GENITOR_STYLE, // replace worst k popluation
    PRESELECTION // replace one of the parents
} Replacement_strategy;
Replacement_strategy replacementType = GENITOR_STYLE; */



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

// calculates fitness array
void calculate_fitness();

// GA operators
void qualify_chromosome(Chromosome &ch); // get quality of particular chromosome (=evaluate cuts)
void qualify_popluation(); // get quality of population (all chromosomes)
void selection(int &p1, int &p2); // selection of chromosome
void crossover(const Chromosome &p1, const Chromosome &p2, Chromosome &offspring); // crossover and create new chromosome from two parents
void mutation(Chromosome &ch); // mutation of chromosome
void replacement(Chromosome* offsprings); // replacement of population(?)

// time checkers (to comply time restrictions)
bool check_timeout(); // check 3 minutes (proj timeout)

// main functions
void ga(); // ga 1 generation

// logging functions
void check_generation(); // check generation count
void check_operation(); // check operation count

// final report
void report_result(); // reports the final best solution
float get_mean(); // gets mean quality of population
float get_standard_deviation(); // gets standard deviation of population quality

// test & print functions
void print_chromosome(const Chromosome* ch);
void print_population();
void print_graph();
void print_result();
void test_selection();
void test_crossover();
void test_mutation();
void test_replacement();

/*
 * Function implementations
 */

void calculate_fitness()
{
    int worst_cost = population[0].quality;
    int best_cost = population[n-1].quality;

    // flipped original formula to encode quality -> cost
    for(int i=0; i<n; i++){
        fitness[i] = (float)((population[i].quality - worst_cost) + (best_cost - worst_cost)) / (float)(4 - 1);
        // k = 4 (maybe need to tuned?)
    }
}

void selection(int &p1, int &p2)
{
    int temp_p1, temp_p2;
    temp_p1 = temp_p2 = -1;

    if(selectionType == ROULETTE_WHEEL)
    {
        /*
        * roulette wheel algorithm
        */
        qsort(population, n, sizeof(Chromosome), compare_quality);
        calculate_fitness();

        float sumOfFitness = 0.0;
        for(int i=0; i<n; i++){
            sumOfFitness += fitness[i];
        }

        int point = rand() % ((int)sumOfFitness - 1);
        int sum = 0;
        for(int i=0; i<n; i++){
            sum = sum + (int)fitness[i] + 1;
            if(point < sum){
                temp_p1 = i;
                break;
            }
        }
        if (temp_p1 == -1) cerr << "error on rw selection 1" << endl;

        point = rand() % ((int)sumOfFitness - 1);
        sum = 0;
        for(int i=0; i<n; i++){
            sum = sum + (int)fitness[i] + 1;
            if(point < sum){
                temp_p2 = i;
                break;
            }
        }
        if (temp_p2 == -1) cerr << "error on rw selection 2" << endl;
    
        if(temp_p1==temp_p2 && temp_p2>0) temp_p2--;
        else temp_p2++;

        p1 = temp_p1;
        p2 = temp_p2;
    }
    else if (selectionType == RANDOM)
    {
        /*
        * random pick of two parents
        */
        temp_p1 = rand() % n; // get random index of population to select parent
        do{
            temp_p2 = rand() % n; // p1,p2 must be different
        }while(temp_p1==temp_p2);

        p1 = temp_p1;
        p2 = temp_p2;
    }

}

void crossover(const Chromosome &p1, const Chromosome &p2, Chromosome &offspring)
{
    /*
     * 1-point crossover
     */
    
    // TODO: maybe remove memset?
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
     */

    // 1. sort solutions by quality
    if(selectionType != ROULETTE_WHEEL) // if is not already sorted
        qsort(population, n, sizeof(Chromosome), compare_quality);

    // 2. replace first k arrays with offsprings
    for(int i=0; i<k; i++)
        memcpy(&population[i], &offsprings[i], sizeof(offsprings[i]));

}

bool check_timeout()
{
    /*
     * timer to check the time time constraint
     * true - over timeout / false - under timeout
     */

    currentTime = clock();
    float timeDuration = (currentTime - startTime) / CLOCKS_PER_SEC;
    return (timeDuration > timeConstraint);

}

float get_mean()
{
    int sum = 0;

    for(int i=0; i<n; i++){
        sum+=population[i].quality;
    }

    return (float)sum/(float)n;
}

float get_standard_deviation()
{
    int sum = 0;
    for(int i=0; i<n; i++){
        sum+=population[i].quality;
    }

    float mean = (float)sum/(float)n;
    float sqsum = 0.0;
    for(int i=0; i<n; i++){
        sqsum += (abs(population[i].quality - mean) * abs(population[i].quality - mean));
    }

    return sqrt(sqsum / (n-1));
}

void report_result()
{
    printf("========== print final result ==========\n");
    printf("Generations evolved: %d\n",generation_count);
    printf("Best quality: %d\n",population[n-1].quality);
    printf("Mean quality: %f\n",get_mean());
    printf("Standard deviation: %f\n",get_standard_deviation());
    printf("======================================\n\n");
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
    for(int i=0; i<vertexNum; i++){
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

    do {

        Chromosome offsprings[k];
        // TODO: maybe remove memset?
        memset( offsprings, 0, sizeof(Chromosome)*k ); // initialize the size of offsprings

        // iterate ga operators for k times
        for (int i=0; i<k; i++){
            int p1, p2;
            selection(p1, p2);
            crossover(population[p1],population[p2],offsprings[i]);
            mutation(offsprings[i]);
        }
        replacement(offsprings);
        generation_count++;        

    } while (!check_timeout());

    report_result();

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
     
    // replacement test
    // replace k 0000... chromosomes with population
    Chromosome test_chs[k];
    // TODO: maybe remove memset?
    memset(test_chs, 0x00, sizeof(Chromosome)*k);
    for(int j=0; j>k; j++){

        for(int i=0; i<vertexNum; i++){
            test_chs[j].gene[i] = 0;
        }
        qualify_chromosome(test_chs[j]);

    }

    replacement(test_chs);
    print_population();
 
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
    // TODO: reset mem of the edges[] & Chromosome.gene (maybe?)
    // is it OK to give longer size of the array and use less?

    // marks start time of the program
    startTime = clock();

    // sets random timer to srand()
    srand(time(NULL));

    // TODO: maybe remove memset?
    memset( population, 0, sizeof(Chromosome)*n );
    init_popluation();
    //print_population(); // just for test.

    generation_count = 0;

}

void print_result()
{
    for(int i=0; i<vertexNum; i++){
        if(population[n-1].gene[i]) printf("%d ",i+1);
    }
}

// just for tests. need to be replaced
int main(int argc, char** argv)
{

    /*
     * File read test
     */

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " GRAPHFILE" << endl;
        return 1;
    }

    read_file(argv[1]);
    init();
    ga();
    print_population();
    report_result();
    print_result();

    //print_graph();
    //test_selection();
    //test_crossover();
    //test_mutation();

    return 0;

}
