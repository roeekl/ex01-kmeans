from fileinput import filename
from msilib.schema import File
import string
import math
import random
import sys
import argparse
from tokenize import String
from turtle import distance
from typing import List
from unittest.mock import DEFAULT

EPSILON=0.001
DEFAULT_ITER=600

def get_points(filename:string)->list:
    with open(filename,'r') as points_file:
         floats_array=[]
         temp_array=[]
         points_array=points_file.readlines()
         result = [x[:-1] for x in points_array]
         for point in result:
            coordinates=point.split(",")
            for coord in coordinates:
                temp_array.append(float(coord))
            floats_array.append(temp_array)
            temp_array=[]    
         return floats_array

def K_algorithm(points_array:float,centorids_array:float,max_iter:int,dim:int)->List:
    min=sys.maxsize
    i=0
    distance=0
    balanced=False
    for iter in range(0,max_iter):
        centroids_distances_array=[[] for _ in range(len(centorids_array))]
        for point in points_array:
            for centroid in centorids_array:
                distance=calculate_distance(point,centroid,dim)
                if(distance<min):
                    min=distance
                    index=i
                i+=1
            centroids_distances_array[index].append(point)
            min=sys.maxsize
            i=0
        centorids_array=calculate_avarge_distance(centroids_distances_array,centorids_array,dim)
    return centorids_array

def calculate_distance(point:list,centroid:list,dim:int)->float:
    sum=0.0
    for cord in range(0,dim):
        sum+=math.pow(abs(point[cord]-centroid[cord]),2)
    distance=math.sqrt(sum)
    return distance

def calculate_avarge_distance(centroids_distances_array,centroids_array,dim:int)->list:
    Index=0
    num_of_points=1
    for centroid in centroids_distances_array:
        centroids_new_locations=[0 for i in range(dim)]
        num_of_points=len(centroid)
        if num_of_points==0:continue
        for point in centroid:
            i=0
            for coord in point:
                centroids_new_locations[i]+=coord
                i+=1
        for j in range(dim):
            centroids_new_locations[j]=centroids_new_locations[j]/num_of_points
        if calculate_distance(centroids_new_locations,centroids_array[Index],dim)>EPSILON:
            centroids_array[Index]=centroids_new_locations
        Index+=1
    
    return centroids_array
                
def write_points(centroids_array:List):
     f = open("centroids_file.txt", "w")
     for centroid in centroids_array:
         f.write(",".join(["%.f4".format(v) for v in centroid])+"\n")
     f.close




def main():
    #parser = argparse.ArgumentParser()
    #parser.add_argument("K", type=int, help="Number of centroids - integer >=1")
    #if len(sys.argv) == 3:
    #    parser.add_argument("MAX_ITER", type=int, help="Maximum number of iterations to reach convergence - integer >=1")
    #elif len(sys.argv) == 2:
    max_iter = DEFAULT_ITER
    #else:
    #    print("Invalid Input!")
    #    exit(1)
    #args = parser.parse_args()
    #K = args[0]
    #if len(sys.argv) == 3:
    #     max_iter = args[1]
    #check = False
    #if K < 1:
    #    print("Invalid Input!")
    #    check = True
    #if max_iter < 1:
    #    print("Invalid Input!")
    #    check = True
    #if check:
    #    exit(1)
    points_array=get_points("./resources/input_3.txt")
    dim=len(points_array[0])
    k=15
    centorids_array=random.sample(points_array,k)
    centorids_array=K_algorithm(points_array,centorids_array,max_iter,dim)
    #output_File = open("output.txt", "w") 
    #write_points(centorids_array)
    for v in centorids_array:
        v = ['{:.4f}'.format(x) for x in v]
        print(','.join(v))
    return 0
main()

