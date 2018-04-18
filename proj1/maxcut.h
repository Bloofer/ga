#include <iostream>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

/*
 * Function definitions
 */

// functions getting input info
void read_file();

// main functions
void init_popluation(); // initialize popluation to start evolution
void evolve(); // evolve 1 generation
