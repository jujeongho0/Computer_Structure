#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Point1 {
	int x;
	int y;
};

struct Point2 {
	int* x;
	int* y;
};

int is_large(int num1, int num2)
{

	return num1 > num2;
}


int sum_x(int x1, int x2)
{
	int sum = 0;

	sum = x1 + x2;

	return sum;
}

void sum_y(int y1, int y2, int* sum)
{
	*sum = y1 + y2;
}


void Point1_to_Point2(struct Point1* P1, struct Point2* P2)
{
	P2->x = &(P1->x);
	P2->y = &(P1->y);
}


void Point2_to_Point1(struct Point1* P1, struct Point2* P2)
{
	P1->x = *P2->x;
	P1->y = *P2->y;
}

int calc_area1(struct Point1* P1, struct Point1* P2)
{
	int area = 0;
	int row, col;
	if (is_large(P1->x, P2->x) == 1)
		row = P1->x - P2->x;
	else
		row = P2->x - P1->x;

	if (is_large(P1->y, P2->y) == 1)
		col = P1->y - P2->y;
	else
		col = P2->y - P1->y;

	area = row * col;

	return area;
}

void calc_area2(struct Point2* P1, struct Point2* P2, int* area)
{
	int row, col;
	if (is_large(*P1->x, *P2->x) == 1)
		row = *P1->x - *P2->x;
	else
		row = *P2->x - *P1->x;

	if (is_large(*P1->y, *P2->y) == 1)
		col = *P1->y - *P2->y;
	else
		col = *P2->y - *P1->y;

	*area = row * col;
}

char* reverse(char* word)
{
	char tmp;
	int len = strlen(word);
	for (int i = 0; i < len / 2; i++)
	{
		tmp = word[i];
		word[i] = word[len - 1 - i];
		word[len - 1 - i] = tmp;
	}
	return word;
}

int main(int argc, char** argv)
{
	int sum = 0, area = 0;
	int x1, x2, y1, y2;
	char word[32];


	if (argc < 5) {
		printf("Usage: %s x1 y1 x2 y2\n", argv[0]);
		return -1;
	}

	x1 = atoi(argv[1]);
	y1 = atoi(argv[2]);
	x2 = atoi(argv[3]);
	y2 = atoi(argv[4]);

	printf("x1: %d, y1: %d, x2: %d, y2: %d\n", x1, y1, x2, y2);

	

	struct Point1 P1 = { .x = x1, .y = y1 };
	struct Point2 P2 = { .x = &x2, .y = &y2 };
	struct Point2 P1_tmp = { .x = NULL, .y = NULL };
	struct Point1 P2_tmp = { .x = 0, .y = 0 };





	// P1. sum = x1 + x2
	sum = sum_x(x1, x2);
	printf("sum_x: %d\n", sum);

	// P2. sum = y1 + y2
	sum_y(y1, y2, &sum);
	printf("sum_y: %d\n", sum);

	// P3. Convert variable type from struct Point2 to struct Point1
	Point2_to_Point1(&P2_tmp, &P2);

	// P4. Calculate Area with struct Point1 type
	area = calc_area1(&P1, &P2_tmp);
	printf("calc_area1: %d\n", area);

	// P5. Convert variable type from struct Point1 to struct Point2
	Point1_to_Point2(&P1, &P1_tmp);

	// P6. Calculate Area with struct Point2 type
	calc_area2(&P1_tmp, &P2, &area);
	printf("calc_area2: %d\n", area);

	// P7. Reverse a word 
	strcpy(word, "erutcetihcra retupmoc evol i");
	printf("The reverse is %s\n", reverse(word));

	P1_tmp.x = NULL;
	P1_tmp.y = NULL;
	free(P1_tmp.x);
	free(P1_tmp.y);
	return 0;
}





