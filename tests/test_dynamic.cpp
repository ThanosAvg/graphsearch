#include <iostream>
#include "../src/dynamic_graph.h"
//#define SIMPLE

int main(){
#ifdef SIMPLE
    DynamicGraph dg;
    dg.add(1,2);
    dg.add(2,4);
    dg.add(3,2);
    dg.add(5,6);
    dg.add(6,7);
    dg.add(8,6);
    dg.add(9,10);
    dg.add(10,6);
    dg.add(11,12);
    dg.estimateConnectedComponents();
    dg.postAdd(10,11);
    dg.postAdd(13,14);
    dg.postAdd(1,6);
    dg.postAdd(1,14);
    dg.postAdd(11,13);
    for(int i = 1; i <= 14; i++){
        std::cout << "Node: " << i << " " << dg.getCC()->findNodeConnectedComponentID(i) << std::endl;
    }

    if(!dg.getCC()->areConnected(dg.getCC()->findNodeConnectedComponentID(6),dg.getCC()->findNodeConnectedComponentID(11))){
        std::cout << "6 and 11 are not connected (but should be)" << std::endl;
    }
    
    std::cout << "Search1: " << dg.query(1,4) << std::endl;
    std::cout << "Search2: " << dg.query(1,8) << std::endl;
    std::cout << "Search3: " << dg.query(6,11) << std::endl;
    std::cout << "Search4: " << dg.query(10,12) << std::endl;
    std::cout << "Search5: " << dg.query(11,14) << std::endl;
    return 0;
#else
    DynamicGraph graph;
    std::cout << "Graph created" << std::endl;
    // Load dataset file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    uint32_t from = 0, to = 0;
    char status = 0;
    FILE *file = fopen("medium/mediumGraph.txt", "r");
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
    graph.estimateConnectedComponents();

    //if(graph.getCC()->findNodeConnectedComponentID(449448) == 0){
    //    std::cout << graph.getCC()->findNodeConnectedComponentID(449448) << "<-" << std::endl;
    //    getchar();
    //}
    std::cout << "CC :" << graph.getCC()->lastComponent << std::endl;
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
            graph.postAdd(from, to);
        }
        else if(status == 'Q'){
            sscanf(line, "%*c %u %u", &from, &to);
            //std::cout << "query:" << "(" << from << "," << to << ") ";
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
            //break;
            continue;
        }
    }
    std::cout << "Merge Size: " << graph.getCC()->getMergedSize() << std::endl;
    std::cout << "Merge Hits: " << graph.getCC()->mergedHits << std::endl;
    std::cout << "EOF" << std::endl;
    //fclose(file);

    //free(line);
#endif
}
