#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstring>

using namespace std;

// functions getting input info
void read_file();
void get_graph_data();

// main functions
void init_popluation(); // initialize popluation to start evolution
void evolve(); // evolve 1 generation

// GA operators
void qualify_chromosome(); // get quality of particular chromosome (=evaluate cuts)
void qualify_popluation(); // get quality of population (all chromosomes)
void selection(); // selection of chromosome
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