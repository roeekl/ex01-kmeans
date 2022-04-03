#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "point.h"

#define DELIM ','

typedef struct point
{
    double *values;
} point_t;

typedef struct point_list
{
    size_t dim;
    point_t *points;
} point_list_t;

int get_dimension(char *filename)
{
    char current_c = '\0';
    int dim = 0;
    FILE *points_file = fopen(filename, "r");
    while ((current_c = fgetc(points_file)) != '\n')
    {
        if (current_c == DELIM)
        {
            dim++;
        }
    }

    fclose(points_file);
    return dim + 1;
}
size_t get_lines_count(char *filename)
{
    size_t lines = 0;
    char current_c;
    FILE *points_file = fopen(filename, "r");
    while (!feof(points_file))
    {
        current_c = fgetc(points_file);
        if (current_c == '\n')
        {
            lines++;
        }
    }
    return lines;
}
int get_points(char *filename, point_list_t *points)
{
    double *values;
    FILE *points_file;
    size_t points_len = get_lines_count(filename);
    size_t i = 0;
    int j = 0;
    int dim = get_dimension(filename);

    points->dim = dim;
    points->points = malloc(sizeof(point_t) * points_len);
    values = malloc(sizeof(double) * dim * points_len);
    points_file = fopen(filename, "r");
    for (i = 0; i < points_len; i++)
    {
        for (j = 0; j < dim; j++)
        {
            fscanf(points_file, "%lf,", &values[(i * dim) + j]);
        }
        points->points[i].values = &values[i * dim];
        fseek(points_file, 1, SEEK_CUR);
    }
    fclose(points_file);
    return points_len;
}

int write_points(char *filename, point_list_t *centers, size_t points_len)
{
    FILE *points_file;
    size_t i, j;
    points_file = fopen(filename, "w");
    if (NULL == points_file)
    {
        printf("ERROR: %d\n", errno);
    }
    for (i = 0; i < points_len; i++)
    {
        for (j = 0; j < centers->dim; j++)
        {

            fprintf(points_file, "%.4f,", centers->points[i].values[j]);
        }
        fseek(points_file, -1, SEEK_CUR); /* The line contains one extra ',' */
        fprintf(points_file, "\n");
    }
    fclose(points_file);
    return EXIT_SUCCESS;
}

int main()
{
    point_list_t points;
    int points_len = 0;
    int dim = get_dimension("./resources/input_1.txt");
    printf("%d\n", dim);
    points_len = get_points("./resources/input_1.txt", &points);
    printf("%d points.\n", points_len);
    write_points("./resources/out_test.txt", &points, 3);
    return 0;
}
