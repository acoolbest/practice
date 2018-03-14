/*
 Generates SC_Info keys (.sinf and .supp)
 see http://hackulo.us/wiki/SC_Info
*/
#include <stdint.h>
#include <iostream>
using namespace std;
void *create_atom(char *name, int len, void *content);
void *coalesced_atom(int amount, uint32_t name, ...);
void *combine_atoms(char *name, int amount, ...);
void *generate_sinf(int appid, char *person_name, int vendorID, uint32_t & out_length);
void *generate_supp(uint32_t *suppsize);