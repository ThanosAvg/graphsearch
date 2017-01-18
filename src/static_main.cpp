#include "graph.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <sys/time.h>

bool testLoadFile();

int main(){
    if(testLoadFile()){
        std::cout << "Loading graph from file success!" << std::endl;
    }
    else{
        std::cout << "Loading graph from file failed!" << std::endl;
    }
}

bool testLoadFile(){
    StaticGraph graph;

    // Load dataset file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint32_t from = 0, to = 0;
    char type[100];
    char status = 0;
    int count = 0;
    FILE *file = fopen("../tests/medium/mediumGraph.txt", "r");
    //FILE *file = fopen("../tests/large/large.txt", "r");
    if(file == NULL){
        std::cout << "File error" << std::endl;
        return false;
    }

    while((read = getline(&line, &len, file)) != -1){
        sscanf(line, "%c", &status);
        if(status != 'S'){
            sscanf(line, "%u %u", &from, &to);
            graph.add(from, to);
        }
    }

    graph.estimateStronglyConnectedComponents();
    graph.estimateComponentsNeighbors('i');
    graph.estimateComponentsNeighbors('o');
    graph.buildGrailIndex();

    fclose(file);
    file = fopen("../tests/medium/mediumWorkload_static_FINAL.txt", "r");
    //file = fopen("../tests/large/largeWorkload_48000_40.txt", "r");
    if(file == NULL){
        std::cout << "File error" << std::endl;
        return false;
    }

    read = getline(&line, &len, file);
    sscanf(line, "%s", type);
    while((read = getline(&line, &len, file)) != -1){
        sscanf(line, "%c", &status);
        if(status == 'A'){
            sscanf(line, "%*c %u %u", &from, &to);
            graph.add(from, to);
        }
        else if(status == 'Q'){
            sscanf(line, "%*c %u %u", &from, &to);
            std::cout << graph.staticQuery(from, to) << std::endl;
        }
        else if(status == 'F'){
            //std::cout << "burst"  << std::endl;
        }
        else if(status == 'S'){
            std::cout << "End of file,quitting..."  << std::endl;
            break;
        }
        else{
            std::cout << "Uknown parameter,quitting..."  << std::endl;
            break;
        }
    }

    fclose(file);
    free(line);
    return true;

}
