#include <iostream>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstring>

using namespace std;

/*
 * Function definitions
 */

// functions getting input info
void read_file();
void get_graph_data();

// main functions
void init_popluation(); // initialize popluation to start evolution
void evolve(); // evolve 1 generation
