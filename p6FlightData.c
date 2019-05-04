/*********************************************************
 FlightData.c by Donovan Olivarez
 Purpose:
    This file holds functions necessary for manipulating flight data.
 Notes:
    1. Functions from previous assignments are used here for inserting 
        in the proper order.
 *********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p5.h"


/******************** calcArr2400 *******************
 int calcArr2400(int iDepTime2400, int iDurationMins, int iZoneChange)
 Purpose:
 *  Calculate arrival of a flate given its departure time, zone change, and duration
 Parameters:
 *  I int iDepTime2400 - departure time
 *  I int iDurationMins - flight length
 *  I int iZoneChange - how many zones to roll back/forth time?
 Returns:
 *  int Arrival time
 Notes:
 *  None
 *****************************************************/
int calcArr2400(int iDepTime2400, int iDurationMins, int iZoneChange) 
{
    // get the hours and minutes of iDepTime2400, calculate total minutes since midnight.
    int iHours = iDepTime2400 / 100;
    int iMinutes = iDepTime2400 % 100;
    int iTotalMinutes = iHours * 60 + iMinutes;
    
    // get zone change in minutes.
    int iZoneChangeMins = (iZoneChange * 60);
    
    // calculate iArrival and convert from minutes to a 2400 time.
    int iArrival = iTotalMinutes + iDurationMins + iZoneChangeMins;
    int iArrHours2400 = iArrival/60 * 100;
    int iArrMins2400 = iArrival % 60;
    iArrival = iArrHours2400 + iArrMins2400;
    
    return iArrival;    
}


/**************** findAirport ******
 int findAirport(Graph g, char airport[])
 Purpose:
 *  Find an airport by name
 Parameters:
 *  I Graph g - graph to search
 *  I char airport[] - airport to search for
 Returns:
 *  Index of airport or -1 if it couldn't find it
 Notes:
 *  None
 */

int findAirport(Graph g, char airport[]){
    int i;
    for(i = 0; i < g->iNumVertices; i++){
        Vertex V = g->vertexM[i];
        if(strcmp(V.szAirport, airport) == 0){
            return i;
        }
    }
    return -1;
}

/******************** insertFlight *******************
 int insertFlight(Graph graph, Flight flight)
 Purpose:
 *  inserts flights based on flightNr
 Parameters:
 *  I/O Graph graph     - pointer to a graph
 *  I   Flight flight   - flight information to be inserted
 Returns:
 *  int - zero if function works properly.
 Notes:
 *  1. uses insertOrderedLL to insert in the correct order.
 *****************************************************/
int insertFlight(Graph graph, Flight flight)
{
    // get origin points
    int origin, dest;
    origin = findAirport(graph, flight.szOrigin);
    dest = findAirport(graph, flight.szDest);
    
    // insert flights in correct order.
    insertOrderedLL(&graph->vertexM[origin].successorList, flight, origin, dest);
    insertOrderedLL(&graph->vertexM[dest].predecessorList, flight, origin, dest);
    return 0;
}

    

/******************** insertOrderedLL *******************
 EdgeNode * insertOrderedLL(EdgeNode **list, Flight flight, int iOrigin, int iDest)
 Purpose:
 *  finds where to insert specified edge node in the graph.
 Parameters:
 *  I/O     EdgeNode **list - double pointer to a successor or predecessor list.
 *  I       Flight flight   - flight information
 *  I       int iOrigin     - origin vertex
 *  I       int iDest       - destination vertex
 Returns:
 *  EdgeNode *pNew  - pointer to the newly inserted node.
 Notes:
 *  1. This function uses searchLL to find out where in the list to insert the
        node. This makes it easier to insert based on szFlightNr,
 *****************************************************/
EdgeNode * insertOrderedLL(EdgeNode **list, Flight flight, int iOrigin, int iDest)
{
    // pPrecedes gets set to NULL in searchLL if list is empty.
    EdgeNode *pNew, *pFind, *pPrecedes;
    pFind = searchLL(*list, flight.szFlightNr, &pPrecedes);
    
    // if duplicate is found
    if (pFind != NULL)
    {
        return NULL;
    }
    
    // create new edge with flight info.
    pNew = allocNodeLL(*list, flight);
    pNew->iOriginVertex = iOrigin;
    pNew->iDestVertex = iDest;
    
    // edge to be inserted at beginning of the list if pPrecedes is NULL
    if (pPrecedes == NULL)
    {
        pNew->pNextEdge = *list;
        *list = pNew;
    }
    else
    {
        pNew->pNextEdge = pPrecedes->pNextEdge;
        pPrecedes->pNextEdge = pNew;
    }
    return pNew;
}

/******************** searchLL *******************
 EdgeNode *searchLL(EdgeNode *list, char flightNr[], EdgeNode **ppPrecedes)
 Purpose:
 *  Search for the correct place to insert flight information
 Parameters:
 *  I EdgeNode *list            -   pointer to the list specified in insertFlight.
 *  I char flightNr[]           -   string that represents the flight we are looking for. 
 *  I/O EdgeNode  **ppPrecedes  -   double pointer that points to the preceding node.          
 Returns:
 *  EdgeNode *p -   returned if node specified in parameter flightNr is found in the list.
 *  NULL        -   if flightNr is not found.
 Notes:
 *  1. ppPrecedes will hold our previous node.
 *****************************************************/
EdgeNode *searchLL(EdgeNode *list, char flightNr[], EdgeNode **ppPrecedes)
{
    EdgeNode *p;
    // NULL used when list is empty or when we need to insert at beginning.
    *ppPrecedes = NULL;
    // traverse though the list looking for where the key belongs or the end of the list.
    
    for (p = list; p != NULL; p = p->pNextEdge)
    {
        // found flight in list already.
        if (strcmp(flightNr, p->flight.szFlightNr) == 0)
            return p;
        if (strcmp(flightNr, p->flight.szFlightNr) < 0)
            return NULL;
        
        *ppPrecedes = p;
    }
    
    return NULL;
}

/******************** allocNodeLL *******************
 EdgeNode *allocNodeLL(EdgeNode *list, Flight flight)
 Purpose:
 *  allocates a new node with new flight information
 Parameters:
 *  I/O EdgeNode *list  -   pointer to a specified list
 *  I Flight flight     -   flight information, to be inserted
 Returns:
 *  EdgeNode *pNew      -   pointer to a new edge
 Notes:
 *  None
 *****************************************************/
EdgeNode *allocNodeLL(EdgeNode *list, Flight flight)
{
    EdgeNode *pNew;
    pNew = (EdgeNode *) malloc(sizeof(EdgeNode));
    
    if (pNew == NULL)
        errExit("could not allocate nodeLL");
    
    
    pNew->flight = flight;
    pNew->pNextEdge = NULL;
    
    return pNew;
}

/************************* removeLL ***************************
 Purpose:
    This function handles the removal of a node in the linked list.
 Parameters:
    I/O    LinkedList list - pointer to a linked list. In this case, the list of events.
 Returns:
    TRUE   -   returned if node was successfully removed.
    FALSE  -   returned if list is empty.
 Notes:
    - The next node in the list becomes the new head in the list, while 
      the head is freed. 
 *******************************************************************/
int removeLL(EdgeNode *list )
{
    // points to node being removed 
    EdgeNode *pRemove;
    if (list == NULL)
        return FALSE;
    // return the event value via pEvent
    pRemove = list;
    list = list->pNextEdge;
    free(pRemove);
    
    return TRUE;
}

/******************** deleteAirport *******************
void deleteAirport (Graph graph, char szAirport[])
 Purpose:
 *  Deletes an airport based on szAirport
 Parameters:
 *  I/O Graph graph     - pointer to a graph
 *  I   char sAirport   - name of airport to be deleted
 Returns:
 *  N/A
 Notes:
 *  1. All edge nodes that reference the deleted airport
 *     are freed.
 *****************************************************/
void deleteAirport (Graph graph, char szAirport[])
{
    // Goal: Delete airport
    // Need to free all edges to and from the airport (vertex)
    // Mark airport as deleted in the vertex array (update bExists)
    EdgeNode *e;
    int iVertex;
    
    // Traverse through graph to find the correct airport
    for (iVertex = 0; iVertex < graph->iNumVertices; iVertex++)
    {
        // If the vertex is the correct airport name
        if(strcmp(graph->vertexM[iVertex].szAirport, szAirport) == 0)
        {
            // Set the vertex/airport to be deleted
            graph->vertexM[iVertex].bExists = FALSE;
            
            // Free edges that have this airport as a predecessor
            for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
            {
                free(e);
            }
            
            // Free edges that have this airport as a successor
            for (e = graph->vertexM[iVertex].predecessorList; e != NULL; e = e->pNextEdge)
            {
                free(e);
            }
        }      
    }
}

