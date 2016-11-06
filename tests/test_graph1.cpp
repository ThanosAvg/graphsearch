#include "../src/graph.h"
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
    FILE *file = fopen("medium/mediumGraph.txt", "r");
    if(file == NULL){
        std::cout << "File error" << std::endl;
        return false;
    }

    while((read = getline(&line, &len, file)) != -1){
        sscanf(line, "%c", &status);
        if(status != 'S'){
            sscanf(line, "%u %u", &from, &to);
            //std::cout << "(" << from << "," << to << ")" << std::endl;
            graph.add(from, to);
        }
    }

    fclose(file);

    file = fopen("medium/mediumWorkload_FINAL.txt", "r");
    if(file == NULL){
        std::cout << "File error" << std::endl;
        return false;
    }

    while((read = getline(&line, &len, file)) != -1){
        //std::cout << "line:" << line << std::endl;
        sscanf(line, "%c", &status);
        if(status == 'A'){
            sscanf(line, "%*c %u %u", &from, &to);
            //std::cout << "addition:" << "(" << from << "," << to << ")" << std::endl;
            graph.add(from, to);
        }
        else if(status == 'Q'){
            sscanf(line, "%*c %u %u", &from, &to);
            //std::cout << "query:" << "(" << from << "," << to << ")" << std::endl;
            std::cout << graph.query(from, to) << std::endl;
            //graph.query(from, to);
        }
        else if(status == 'F'){
            //std::cout << "RIPH"  << std::endl;
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
