#include "graph.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>

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
    Graph graph;

    // Load dataset file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint32_t from = 0, to = 0;
    char status = 0;
    int count = 0;
    FILE *file = fopen("../tests/small/smallGraph.txt", "r");
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

    fclose(file);

    file = fopen("../tests/small/smallWorkload_FINAL.txt", "r");
    if(file == NULL){
        std::cout << "File error" << std::endl;
        return false;
    }

    while((read = getline(&line, &len, file)) != -1){
        sscanf(line, "%c", &status);
        if(status == 'A'){
            sscanf(line, "%*c %u %u", &from, &to);
            graph.add(from, to);
        }
        else if(status == 'Q'){
            sscanf(line, "%*c %u %u", &from, &to);
            std::cout << graph.query(from, to) << std::endl;
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
