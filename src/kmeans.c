#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#define DELIM ','
#define EPSILON 0.01
#define DEFAULT_ITER 200
#define PRINT_ITERS
#define INVALID_INPUT_ERR "Invalid Input!\n"
#define GENERAL_ERR "Invalid Input!\n"
#define FUNC_SUCCESS 0

size_t dim;

typedef struct point
{
    double *values;
} point_t;

typedef struct clustered_point
{
    point_t point;
    size_t cluster;
} clustered_point_t;

typedef struct cluster
{
    point_t centroid;
    size_t size;
} cluster_t;

size_t get_dimension(char *filename)
{
    char current_c = '\0';
    size_t dim = 0;
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
    fclose(points_file);
    return lines;
}
int get_points(char *filename, point_t **points)
{
    double *values;
    FILE *points_file;
    size_t points_len = get_lines_count(filename);
    size_t i, j;
    dim = get_dimension(filename);
    *points = malloc(sizeof(point_t) * points_len);
    values = malloc(sizeof(double) * dim * points_len);
    points_file = fopen(filename, "r");
    for (i = 0; i < points_len; i++)
    {
        for (j = 0; j < dim; j++)
        {
            fscanf(points_file, "%lf,", &values[(i * dim) + j]);
        }
        (*points)[i].values = &values[i * dim];
        fseek(points_file, 1, SEEK_CUR);
    }
    fclose(points_file);
    return points_len;
}

int write_points(char *filename, cluster_t *clusters, size_t points_len)
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
        for (j = 0; j < dim; j++)
        {
            fprintf(points_file, "%.4f,", clusters[i].centroid.values[j]);
        }
        fseek(points_file, -1, SEEK_CUR); /* The line contains one extra ',' */
        fprintf(points_file, "\n");
    }
    fclose(points_file);
    return FUNC_SUCCESS;
}

int initialize_clusters(cluster_t **clusters, size_t k, point_t *points)
{
    double *values;
    size_t i, j;
    point_t selected_point;

    values = malloc(sizeof(double) * dim * k);
    *clusters = malloc(sizeof(cluster_t) * k);
    for (i = 0; i < k; i++)
    {

        (*clusters)[i].size = 0;
        selected_point = points[i];
        for (j = 0; j < dim; j++)
        {
            values[(i * dim) + j] = selected_point.values[j];
        }
        (*clusters)[i].centroid.values = &values[(i * dim)];
    }

    return FUNC_SUCCESS;
}
double calc_distance(point_t a, point_t b)
{
    double distance = .0;
    size_t i;
    for (i = 0; i < dim; i++)
    {
        distance += pow((a.values[i] - b.values[i]), 2.0);
    }
    return sqrt(distance);
}
size_t find_closest_cluster(point_t point, cluster_t *centroids, size_t k)
{
    size_t i;
    double distance = .0, max_distance = DBL_MAX;
    size_t selected = k + 1;
    for (i = 0; i < k; i++)
    {
        distance = calc_distance(point, centroids[i].centroid);
        if (distance < max_distance)
        {
            max_distance = distance;
            selected = i;
        }
    }
    return selected;
}

int assign_to_clusters(clustered_point_t *points, size_t points_len, cluster_t *clusters, size_t k)
{
    size_t closest;
    size_t i;
    for (i = 0; i < points_len; i++)
    {
        closest = find_closest_cluster(points[i].point, clusters, k);
        points[i].cluster = closest;
        clusters[closest].size++;
    }
    return 0;
}

double update_centroids(clustered_point_t *points, size_t points_len, cluster_t *clusters, size_t k)
{
    size_t i, j;
    size_t cluster;
    double delta = .0, centroid_delta;

    point_t *new_centroids = (point_t *)calloc(k, sizeof(point_t));
    double *axis_sum = (double *)calloc(dim * k, sizeof(double));
    if (NULL == new_centroids || NULL == axis_sum)
    {
        return 1;
    }
    for (i = 0; i < points_len; i++)
    {
        cluster = points[i].cluster;
        for (j = 0; j < dim; j++)
        {
            axis_sum[cluster * dim + j] += points[i].point.values[j] / clusters[cluster].size;
        }
    }
    for (i = 0; i < k; i++)
    {
        new_centroids[i].values = &axis_sum[i * dim];
    }

    for (i = 0; i < k; i++)
    {

        centroid_delta = calc_distance(clusters[i].centroid, new_centroids[i]);
        if (centroid_delta > delta)
        {
            delta = centroid_delta;
        }

        for (j = 0; j < dim; j++)
        {
            clusters[i].centroid.values[j] = axis_sum[i * dim + j];
        }
        clusters[i].size = 0;
    }
    free(axis_sum);
    free(new_centroids);
    return delta;
}

int kmeans(point_t *points, size_t points_len, cluster_t *clusters, size_t k, size_t max_iter)
{
    size_t i;
    clustered_point_t *clustered_points;
    double max_delta;

    clustered_points = (clustered_point_t *)malloc(sizeof(clustered_point_t) * points_len);
    for (i = 0; i < points_len; i++)
    {
        clustered_points[i].point = points[i];
    }
    for (i = 0; i < max_iter; i++)
    {
        assign_to_clusters(clustered_points, points_len, clusters, k);

        max_delta = update_centroids(clustered_points, points_len, clusters, k);
        if (max_delta <= EPSILON)
        {
            break;
        }
    }
    free(clustered_points);
    return FUNC_SUCCESS;
}
int main(int argc, char *argv[])
{
    size_t k, max_iter = DEFAULT_ITER;
    char *input_file, *output_file;
    point_t *points;
    cluster_t *clusters;
    size_t points_len = 0;
    k = atoi(argv[1]);
    if (k <= 0)
    {
        printf(GENERAL_ERR);
        exit(1);
    }
    if (argc == 5)
    {
        max_iter = atoi(argv[2]);
        if (max_iter <= 0)
        {
            printf(GENERAL_ERR);
            exit(1);
        }
        input_file = argv[3];
        output_file = argv[4];
    }
    else if (argc == 4)
    {
        input_file = argv[2];
        output_file = argv[3];
    }
    else
    {
        printf(INVALID_INPUT_ERR);
        exit(1);
    }
    dim = get_dimension(input_file);
    points_len = get_points(input_file, &points);

    initialize_clusters(&clusters, k, points);

    kmeans(points, points_len, clusters, k, max_iter);
    write_points(output_file, clusters, k);
    return 0;
}
