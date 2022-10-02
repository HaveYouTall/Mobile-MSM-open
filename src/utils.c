#include <stdint.h>
#include <stdio.h>
#include "utils.h"


/// @param: nlimbs shows the size of num array.
///        e.g., for unit64_t num[6], nlimbs is 6.
/// @param: num is the big number to be printed.
void PrintBigNumber(uint64_t *num, size_t nlimbs) {
    printf("From the least significant 64 bits to the most significant 64 bits\n");
    for(size_t i = 0; i < nlimbs; i++) {
        printf("%llx\n", num[i]);  
    }
}

void print_u48(const u48* input)
{
	for (int i = 0; i < 6; i++)
	{
		printf("%016llX\n", input->v[i]);
	}
	printf("\n");
}

void print_AFPoint(const AFPoint* p)
{
	int i = 0;
	printf("x = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->x.v[i]);
	}
	printf("\n");

	printf("y = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->y.v[i]);
	}
	printf("\n");
	printf("\n");
}
#include <iomanip>
void store_AFPoint(std::ofstream &out, const AFPoint* p)
{
	out << "x = \n" << "0x" << std::hex;
	for (int i = 5; i>=0; i--) {
		out <<  std::setfill('0') << std::setw(16) << p->x.v[i];
	}
	out << std::endl;

	out << "y = \n" << "0x" << std::hex;
	for (int i = 5; i>=0; i--) {
		out << std::setfill('0') << std::setw(16) << p->y.v[i];
	}
	out << std::endl;
}

void print_JPoint(const JPoint* p)
{
	int i = 0;
	printf("x = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->x.v[i]);
	}
	printf("\n");

	printf("y = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->y.v[i]);
	}
	printf("\n");

	printf("z = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->z.v[i]);
	}
	printf("\n");

#ifdef Edwards
	printf("t = \n");
	for (i = 0; i < 6; i++)
	{
		printf("0x%016llX, ", p->t.v[i]);
	}
	printf("\n");
#endif
	printf("\n");
}

bool checkInfinityFlag(uint8_t value) {
	return value >> 6 & 1 == 1;
}