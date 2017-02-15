#include "dynamic_graph.h"
#include "graph.h"
#include "query_array.h"
#include "job_scheduler.h"

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

int main(int argc, char *argv[]){
    char type[20];
    if(argc != 2){
        std::cout << "You have to specify either dynamic or static (./gsp dynamic or ./gsp static)" << std::endl;
        return -1;
    }
    else if(strcmp(argv[1], "static") != 0 && strcmp(argv[1], "dynamic") != 0){
        std::cout << "You can only select between static and dynamic" << std::endl;
        return -1;
    }
    else{
        strcpy(type, argv[1]);
    }
    if(strcmp(type, "static") == 0){
        std::cout << "STATIC VERSION" << std::endl;
        StaticGraph graph;

        QueryArray qarr(5000, 5000);
        int line = 0;
        int myChar;
        uint32_t from, to;
        int queries = 0;
        int scanret = 0;

        while(1){
            //std::cin.get(myChar);
            myChar = getc(stdin);
            if(myChar == 'A' || myChar == 'Q'){
                ungetc(myChar, stdin); // Fix for problematic datasets
                break;
            }
            if(myChar == 'S' || myChar == EOF){
                break;
            }
            else{
                //std::cin.putback(myChar);
                ungetc(myChar, stdin);
                scanret = scanf("%u %u\n", &from, &to);
                //std::cout << "Adding " << from << " " << to << std::endl;
                graph.add(from, to);
            }
        }

        graph.estimateStronglyConnectedComponents();
        graph.estimateComponentsNeighbors('i');
        graph.estimateComponentsNeighbors('o');
        graph.buildGrailIndex();

        while(1){
            myChar = getc(stdin);
            if(myChar == 'F'){
                qarr.addBurst(queries - 1);
            }
            else if(myChar == EOF){
                break;
            }
            else if(myChar == 'Q'){
                scanret = scanf("%u %u\n", &from, &to);
                qarr.addQuery(from, to, queries, 0);
                queries++;
                //std::cout << "Query " << from << " " << to << std::endl;
                //std::cout << graph.query(from, to) << std::endl;
            }
            else if(myChar == 'A'){
                scanret = scanf("%u %u\n", &from, &to);
                //graph.add(from, to);
            }
            line++;
        }

        JobScheduler *scheduler = new JobScheduler(&qarr, &graph);
        scheduler->parallelQueryExecution();

        /*

        QueryData* qd;
        for(int i = 0; i < queries; i++){
            qd = qarr.getQuery(i);
            std::cout << qd->queryFrom << " " << qd->queryTo << std::endl;
        }
        */
    }
    else{
        std::cout << "DYNAMIC VERSION" << std::endl;
        DynamicGraph graph;

        QueryArray qarr(5000, 5000);
        int line = 0;
        int myChar;
        uint32_t from, to;
        int queries = 0;
        int scanret = 0;
        while(1){
            //std::cin.get(myChar);
            myChar = getc(stdin);
            if(myChar == 'A' || myChar == 'Q'){
                ungetc(myChar, stdin); // Fix for problematic datasets
                break;
            }
            if(myChar == 'S' || myChar == EOF){
                return -1;
                break;
            }
            else{
                //std::cin.putback(myChar);
                ungetc(myChar, stdin);
                scanret = scanf("%u %u\n", &from, &to);
                //std::cout << "Adding " << from << " " << to << std::endl;
                graph.postAddWithVersion(from, to, 0);
            }
        }

        uint32_t currentVersion = 0;
        int prevChar = 'Q';
        while(1){
            myChar = getc(stdin);
            if(myChar == 'F'){
                qarr.addBurst(queries - 1);
            }
            else if(myChar == EOF){
                break;
            }
            else if(myChar == 'Q'){
                scanret = scanf("%u %u\n", &from, &to);
                qarr.addQuery(from, to, queries, currentVersion);
                queries++;
                //std::cout << "Query " << from << " " << to << std::endl;
                //std::cout << graph.query(from, to) << std::endl;
            }
            else if(myChar == 'A'){
                scanret = scanf("%u %u\n", &from, &to);
                if(prevChar == 'Q'){
                    currentVersion++;
                }
                graph.postAddWithVersion(from, to, currentVersion);
            }
            line++;
            prevChar = myChar;
            //putchar(myChar);
            //std::cout << " " << from << " " << to << " VERSION:" << currentVersion << std::endl;
        }
        //graph.estimateConnectedComponents();
        JobScheduler *scheduler = new JobScheduler(&qarr, &graph);
        scheduler->parallelQueryExecution();
    }
}
