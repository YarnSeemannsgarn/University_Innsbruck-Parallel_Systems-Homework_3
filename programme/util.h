#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define RANGE_MAX 10000
#define PRINTF_WIDTH 10
#define PRINTF_PRECISION 4
#define MEMORY_MESSAGE "Not enough memory. Choose a smaller problem size!\n"
#define USAGE_MESSAGE "Usage: %s <problem-size> [<debug>]\n"

int *create_matrix(size_t M, size_t N);
void print_matrix(int *matrix, size_t M, size_t N);

// helpers for matrices
typedef double matrix_value_t;
typedef matrix_value_t ** matrix_t;

#define MATRIX_VALUE_MAX 10000
#define MATRIX_VALUE_MPI_TYPE MPI_DOUBLE

// allocates a single block of memory to allow using it with MPI methods like scatter/gather
matrix_t matrix_create(const unsigned int rows, const unsigned int cols);
void matrix_init(matrix_t matrix, const unsigned int rows, const unsigned int cols);
void matrix_delete(matrix_t matrix);
void matrix_print(const matrix_t matrix, const unsigned int rows, const unsigned int cols);

// allocates a single block of memory to allow using it with MPI methods like scatter/gather
matrix_t matrix_create(const unsigned int rows, const unsigned int cols);
void matrix_init(matrix_t matrix, const unsigned int rows, const unsigned int cols);
void matrix_delete(matrix_t matrix);
void matrix_print(const matrix_t matrix, const unsigned int rows, const unsigned int cols);


// array helpers
typedef long array_value_t;
typedef array_value_t * array_t;

array_t array_create(const unsigned int size);
void array_init(array_t array, const unsigned int size);
void array_delete(array_t array);

void array_swap(array_t array, const unsigned int i, const unsigned int j);
int array_check_sorted(const array_t array, const unsigned int size);

// helpers for timing and measuring performance
typedef double timepoint_t;

void timer_start(timepoint_t * time);
// returns difference between start and now in milliseconds
unsigned long timer_end(const timepoint_t * start);
void timer_print_end(const timepoint_t * start);


// static assertion macro
// code from http://www.pixelbeat.org/programming/gcc/static_assert.html
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
#define STATIC_ASSERT(e,m) \
  ;enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
/* This can't be used twice on the same line so ensure if using in headers
 * that the headers are not included twice (by wrapping in #ifndef...#endif)
 * Note it doesn't cause an issue when used on same line of separate modules
 * compiled with gcc -combine -fwhole-program.  */
#define STATIC_ASSERT(e,m) \
  ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif

#endif
