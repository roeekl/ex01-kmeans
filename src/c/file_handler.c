#include <stdio.h>
#include <stdlib.h>
#include "point.h"

#define DELIM ','

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
int get_points(char *filename, point_t **points)
{
    double *values;
    FILE *points_file;
    size_t points_len = get_lines_count(filename);
    size_t i = 0;
    int j = 0;
    int dim = get_dimension(filename);
    *points = malloc(sizeof(point_t) * points_len);
    values = malloc(sizeof(double) * dim * points_len);
    points_file = fopen(filename, "r");
    for (i = 0; i < points_len; i++)
    {
        points[i]->dim = dim;
        for (j = 0; j < dim; j++)
        {
            fscanf(points_file, "%lf,", &values[(i * dim) + j]);
        }
        points[i]->values = &values[i];
        fseek(points_file, 1, SEEK_CUR);
    }
    fclose(points_file);
    return points_len;
}

int main()
{
    point_t **points;
    int points_len = 0;
    int dim = get_dimension("./resources/input_1.txt");
    printf("%d\n", dim);
    points = malloc(sizeof(point_t));
    points_len = get_points("./resources/input_1.txt", points);
    printf("%d points.\n", points_len);
    return 0;
}
