#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

int index_for_storing_edges =0;

void printRoutingTables(graph* topology){
    int v = topology->numVertex;
    for(int i=0;i<v;i++){
//        printf("%s",topology->routersArray[i].routerName);
        printf("\nRouting Table for %s \n",topology->routersArray[i].routerName);
        for(int j=0;j<v;j++){
            printf("\t%s      %d\n",topology->routersArray[i].rt.destinationRouters[j].routerName, topology->routersArray[j].rt.costArray[j]);
        }
    }
}

void printEdges(graph* topology,int numEdges){

    printf("\n\nPrinting All the Edges:\n");
    for(int i=0;i<numEdges;i++){
        edge link = topology->edgesArray[i];
        printf("\n\tNode Name : %s ::\n\t Neighbour Node: %s,\n\t cost = %u\n",
               link.intf1.attachedNode->routerName,link.intf2.attachedNode->routerName, link.cost);
   }
}


node *getNeighbourNode(interface *intf) {    // Each edge has two ends which means 2 nodes, so this function returns the node that is on other end of the edge.
    edge *link = intf->attachedEdge;
    if(&link->intf1 == intf)
        return link->intf2.attachedNode;
    else
        return link->intf1.attachedNode;
}

int getEmptyInterfaceSlot(node *router){

    for(int i=0 ; i<MAXIMUM_INTERFACE_PER_NODE;i++){
        if(router->intf[i] != NULL)
            continue;
        return i;
    }
    return -1;
}

void printInterface(interface *intf){

    edge *link = intf->attachedEdge;
    node *neighbourNode = getNeighbourNode(intf);

    printf("\n\tInterface Name : %s ::\n\t Neighbour Node: %s,\n\t cost = %u\n",intf->interfaceName,neighbourNode->routerName, link->cost);
}

void printGraph(graph* topology){
    int v = topology->numVertex;
    int e = topology->numEdges;
    interface *intf;

    printf("\n\n Traversing Network Topology:\n");
    for(int i=0;i<v;i++){
        printf("%s ->",topology->routersArray[i].routerName);   // printing router name;
        for(int j=0;j<e;j++){
            intf = topology->routersArray[i].intf[j];
            if(!intf)
                break;
            printInterface(intf);                   // function to print details of connected Interface.
        }
    }
}
void addEdge(graph *topology ,node* node1 , node* node2, char* from_, char* to_, int cost){

    edge *link = (edge*) malloc(sizeof(edge));

    // Setting name of each interface in the edge structure.
    strncpy(link->intf1.interfaceName, from_ ,NAME_SIZE);
    link->intf1.interfaceName[NAME_SIZE-1] = '\0';
    strncpy(link->intf2.interfaceName, to_ ,NAME_SIZE);
    link->intf2.interfaceName[NAME_SIZE-1] = '\0';

    link->intf1.attachedEdge = link;          // Setting back pointer to the edge structure.
    link->intf2.attachedEdge = link;        // this will help in determining the neighbour of the each interface;

    link->intf1.attachedNode = node1;
    link->intf2.attachedNode = node2;

    link->cost = cost;

    int emptyInterfaceSlot;
    emptyInterfaceSlot = getEmptyInterfaceSlot(node1);
    if(emptyInterfaceSlot == -1){
        printf("Cannot add more edges to this Node.");
        exit(0);
    }
    node1->intf[emptyInterfaceSlot] = &link->intf1;
    emptyInterfaceSlot = getEmptyInterfaceSlot(node2);
    if(emptyInterfaceSlot == -1){
        printf("Cannot add more edges to this Node.");
        exit(0);
    }
    node2->intf[emptyInterfaceSlot] = &link->intf2;
    topology->edgesArray[index_for_storing_edges++] = *link;
}

void initializeRoutingTables(graph* topology){
    int v = topology->numVertex;

    for(int i=0;i<v;i++){
        for(int j=0;j<v;j++) {
            topology->routersArray[i].rt.destinationRouters[j] = *(topology->routersArray[j].intf[0]->attachedNode);
            topology->routersArray[i].rt.costArray[j] = INT_MAX;
        }
    }
}

void createRoutingTable(routing_table* rt,int numVertex){
    rt->destinationRouters = (node*) malloc(numVertex * sizeof(node));
    rt->costArray = (int *) malloc(numVertex * sizeof(int));
}

void createGraphNodes(node* router, char *name){
    strncpy(router->routerName,name,32);
    name[NAME_SIZE-1] = '\0';
}

graph *createGraph(int vertex,int edges){

    graph* topology = (graph*) malloc(sizeof(graph));
    topology->numVertex = vertex;
    topology->numEdges = edges;

    topology->routersArray = (node*) malloc(vertex* sizeof(node));		// array of nodes
    
    topology->edgesArray =   (edge*) malloc(edges* sizeof(edge));	// array holding edges
    
    for(int i=0;i<vertex;i++){
        char name[] = "RouterX";
        name[6] = i+'0';
        createRoutingTable(&topology->routersArray[i].rt, topology->numVertex);
        createGraphNodes(&topology->routersArray[i], name);
    }

    return topology;
}
