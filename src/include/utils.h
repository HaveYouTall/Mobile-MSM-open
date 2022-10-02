#ifndef _UTILS_H_
#define _UTILS_H_
#include <fstream>
#include <iostream>
#include "dataType.h"
/// @param: nlimbs shows the size of num array.
///        e.g., for unit64_t num[6], nlimbs is 6.
/// @param: num is the big number to be printed.
void PrintBigNumber(uint64_t *num, size_t nlimbs);

void print_u48(const u48* input);
void print_AFPoint(const AFPoint* p);
void print_JPoint(const JPoint* p);

void store_AFPoint(std::ofstream &out, const AFPoint* p);

bool checkInfinityFlag(uint8_t value);

#endif