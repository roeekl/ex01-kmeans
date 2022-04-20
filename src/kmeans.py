import math
import sys
from typing import List, Tuple

EPSILON = 0.001
DEFAULT_ITER = 600
INVALID_INPUT_MESSAGE = "Invalid Input!"
GENERAL_ERROR_MESSAGE = "An Error Has Occurred"


def get_points(filename: str) -> List[List[float]]:
    with open(filename, "r") as points_file:
        floats_array = []
        temp_array = []
        points_array = points_file.readlines()
        result = [x[:-1] for x in points_array]
        for point in result:
            coordinates = point.split(",")
            for coord in coordinates:
                temp_array.append(float(coord))
            floats_array.append(temp_array)
            temp_array = []
        return floats_array


def kmeans(
    points_array: float, centorids_array: float, max_iter: int, dim: int
) -> List[float]:
    min = sys.maxsize
    i = 0
    distance = 0
    for _ in range(0, max_iter):
        centroids_distances_array = [[] for _ in range(len(centorids_array))]
        for point in points_array:
            for centroid in centorids_array:
                distance = calculate_distance(point, centroid)
                if distance < min:
                    min = distance
                    index = i
                i += 1
            centroids_distances_array[index].append(point)
            min = sys.maxsize
            i = 0
        centorids_array, centroids_delta = calculate_avarge_distance(
            centroids_distances_array, centorids_array, dim
        )
        if centroids_delta < EPSILON:
            break
    return centorids_array


def calculate_distance(point: list, centroid: list) -> float:
    sum = 0.0
    for v1, v2 in zip(point, centroid):
        sum += math.pow(abs(v1 - v2), 2)
    distance = math.sqrt(sum)
    return distance


def calculate_avarge_distance(
    centroids_distances_array, centroids_array, dim: int
) -> Tuple[List[float], float]:
    num_of_points = 1
    result_centroids = []
    for centroid in centroids_distances_array:
        centroids_new_locations = [0 for i in range(dim)]
        num_of_points = len(centroid)
        if num_of_points == 0:
            continue
        for point in centroid:
            i = 0
            for coord in point:
                centroids_new_locations[i] += coord
                i += 1
        for j in range(dim):
            centroids_new_locations[j] = centroids_new_locations[j] / num_of_points
        result_centroids.append(centroids_new_locations)

    max_delta = max(
        [
            calculate_distance(c1, c2)
            for c1, c2 in zip(centroids_array, result_centroids)
        ]
    )
    return result_centroids, max_delta


def write_points(output_file: str, centroids_array: List[List[float]]):
    with open(output_file, mode="w") as output_f:
        for centroid in centroids_array:
            output_f.write(",".join([f"{v:.4f}" for v in centroid]) + "\n")


def get_positive_int(input: str) -> int:
    try:
        result = int(input)
        if result <= 0:
            raise ValueError()
    except ValueError:
        print(GENERAL_ERROR_MESSAGE)
        exit()
    return result


def main():
    args = sys.argv[1:]
    k = get_positive_int(args[0])
    if len(args) == 3:
        max_iter = DEFAULT_ITER
        input_path = args[1]
        output_path = args[2]
    elif len(args) == 4:
        max_iter = get_positive_int(args[1])
        input_path = args[2]
        output_path = args[3]
    else:
        print(INVALID_INPUT_MESSAGE)
        exit()
    points = get_points(input_path)

    dim = len(points[0])
    centorids = points[:k]
    centorids = kmeans(points, centorids, max_iter, dim)
    write_points(output_path, centorids)


if __name__ == "__main__":
    main()
