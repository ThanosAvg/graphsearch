#include "../src/graph.h"
#include <iostream>
#include <stdio.h>

bool testLoadFile();

int main(){
    if(testLoadFile()){
        std::cout << "Loading graph from file success" << std::endl;
    }
    else{
        std::cout << "Loading graph from file failed" << std::endl;
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
    FILE *file = fopen("tinyGraph.txt", "r");
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

    return true;
}
