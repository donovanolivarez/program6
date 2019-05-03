/*********************************************************
 p6graph.c by Donovan Olivarez
 Purpose:
    This file holds functions necessary to the creation of the graph.
 Notes:
    
 *********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

// TO DO: download and add p6 header file to project.
#include "cs2123p6.h"


/******************** newGraph *************************
 Graph newGraph()
 Pupose:
    creates and returns a new graph.
 Parameters:
 
 Returns:
    Graph graph  - a pointer to a new graph.
 Notes:
 
 ********************************************************/
Graph newGraph()
{
    // alloc space for a new graph
    Graph graph = malloc(sizeof(GraphImp));
    return graph;
}

/******************** setNotVisited *************************
 void setNotVisited(Graph graph)
 Pupose:
    sets all bVisited flags to false
 Parameters:
    I/O  Graph g   -   a pointer to the graph.
 Returns:
    None
 Notes:
 
 ********************************************************/
void setNotVisited(Graph graph)
{
    // loop through vertices, set all flags to false
    int i;
    for (i = 0; i < graph->iNumVertices; i++)
    {
        graph->vertexM[i].bVisited = FALSE;
    }
}

/******************** addVertex *************************
 void addVertex(Graph g, char airport[])
 Pupose:
    add a vertex that does not exist.
 Parameters:
   I/O   Graph g       -   a pointer to the graph.
    I   char airport   -   string representing an airport name.
 Returns:
    None
 Notes:
 
 ********************************************************/
void addVertex(Graph g, char airport[])
{
    // adds a new vertex to the graph.
    memcpy(g->vertexM[g->iNumVertices].szAirport, airport, 5);
    g->vertexM[g->iNumVertices].predecessorList = NULL;
    g->vertexM[g->iNumVertices].successorList = NULL;
    g->iNumVertices++;
}

/******************** freeGraph *************************
 void freeGraph(Graph g)
 Pupose:
    free nodes in graph
 Parameters:
   I/O   Graph g       -   a pointer to the graph.
 Returns:
    None
 Notes:
    uses removeLL to remove node
 ********************************************************/
void freeGraph(Graph g)
{
    // to iterate through each vertex
    int i;
    EdgeNode *e;
    
    for (i = 0; i < g->iNumVertices; i++)
    {
        for (e = g->vertexM[i].successorList; e != NULL; e = e->pNextEdge)
        {
            removeLL(e);
        }
        
        for (e = g->vertexM[i].predecessorList; e != NULL; e = e->pNextEdge)
        {
            removeLL(e);
        }

    }
    free(g);
}

