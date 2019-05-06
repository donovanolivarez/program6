/*************************************************************** 
 PrintFunctions.c By Angela Humphries
 Purpose:
    This source file holds all of the functions necessary for printing out
    flight information. prtTraversal and prtTraversalChron are done recursively.
    The remainder of the print functions are iterative.
 Notes:
 
****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p5.h"

/************************ prtTraversal **************************
void prtTraversal(Graph graph, int iAirportVertex, int indent)
Purpose:
   Prints all the successors of a specified airport in a depth 
   first manner with indentation.
Parameters:
   I  Graph graph        - a pointer to the graph of flights/airports
   I  int iAirportVertex - the number of the current airport
   I  int indent         - amount of indents to be printed
Returns:
    N/A
Notes:
   1.  This function uses the vertex successor list to print 
       the correct data.
   2.  A warning will be printed if the airport does not exist.
   3.  This function is recursive by a depth first manner.
 ****************************************************************/
void prtTraversal(Graph graph, int iAirportVertex, int indent)
{
    /****  print out successors using depth first ****/
    EdgeNode *eSuccessor, *ePred;
    
    // if successor list is empty, return
    if (graph->vertexM[iAirportVertex].successorList == NULL)
        return;
    
    // if vertex has been visited, return. Set vertex to visited.
    if (graph->vertexM[iAirportVertex].bVisited == TRUE)
        return;
    
    graph->vertexM[iAirportVertex].bVisited = TRUE;
    
    // loop through successors, print indentations.
    int i, arrival, nextVertex;
    for (eSuccessor = graph->vertexM[iAirportVertex].successorList; eSuccessor != NULL;  eSuccessor = eSuccessor->pNextEdge)
    {
 
        if ((graph->vertexM[eSuccessor->iDestVertex].bVisited == TRUE) && (strcmp(graph->vertexM[eSuccessor->iDestVertex].szAirport, eSuccessor->flight.szDest) == 0))
            continue;
        
        for (i = 0; i < indent; i++)
        {
            printf("    ");
        }
        
        // print current info, make recursive call, increase indent by 1.
        arrival = calcArr2400(eSuccessor->flight.iDepTm2400, eSuccessor->flight.iDurationMins, eSuccessor->flight.iZoneChange);
        nextVertex = findAirport(graph, eSuccessor->flight.szDest);
        printf("%s-%s %d-%d\n", eSuccessor->flight.szFlightNr, eSuccessor->flight.szDest, eSuccessor->flight.iDepTm2400, arrival);
        prtTraversal(graph, nextVertex, indent + 1);
        
    }

}
    

/************************ prtTraversalChron **************************
void prtTraversalChron(Graph graph, int iVertex, int iIndent, int iPrevArrTm2400)
Purpose:
   Prints all the successors of a specified airport in a depth 
   first manner which have a time reasonably after the preceding flight
   and with indentation.
Parameters:
   I  Graph graph        - a pointer to the graph of flights/airports
   I  int iAirportVertex - the number of the current airport
   I  int indent         - amount of indents to be printed
 * I  int iPreArrTm2400  - time that the last flight arrived at
Returns:
    N/A
Notes:
   1.  This function uses the vertex successor list to print 
       the correct data.
   2.  A warning will be printed if the airport does not exist.
   3.  This function is recursive by a depth first manner.
 ****************************************************************/
void prtTraversalChron(Graph graph, int iVertex, int iIndent, int iPrevArrTm2400)
{
    /****  print out successors using depth first ****/
    EdgeNode *e;
    
    // if vertex successor list is NULL, return.
    if (graph->vertexM[iVertex].successorList == NULL)
        return;
    
    // if vertex has been visited, return. If not, vertex will be set to visited.
    if (graph->vertexM[iVertex].bVisited == TRUE)
        return;
    graph->vertexM[iVertex].bVisited = TRUE;
    
    // traverse successors
    int i, arrival, nextVertex;
    for (e = graph->vertexM[iVertex].successorList; e != NULL;  e = e->pNextEdge)
    {
        
        // if vertex has been visited and the origin airport is found as a successor, do not print.
        if ((graph->vertexM[e->iDestVertex].bVisited == TRUE) && (strcmp(graph->vertexM[e->iDestVertex].szAirport, e->flight.szDest) == 0))
            continue;
        
        // if previous arrival time is greater than current flight departure time, not a valid successor. 
        if (iPrevArrTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS > e->flight.iDepTm2400)
            continue;
        
        //print spaces
        for (i = 0; i < iIndent; i++)
        {
            printf("    ");
        }
        
        // calc arrival time 
        arrival = calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange);
        
        // calc next vertex. Make recursive call using arrival time. Increase indent by 1.
        nextVertex = findAirport(graph, e->flight.szDest);
        printf("%s-%s %d-%d\n", e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, arrival);
        prtTraversalChron(graph, nextVertex, iIndent + 1, arrival );

    }
}

/************************** prtAll *******************************
void prtAll(Graph graph)
 Purpose:
    Print all the airports as is done in PRTONE.
 Parameters:
    I  Graph graph - a pointer to the graph of flights/airports
 Returns:
    N/A
 Notes:
    1.  A warning will be printed if the airport does not exist.
 ****************************************************************/
void prtAll(Graph graph)
{
    // call prtOne for all vertices
    int i;
    for (i = 0; i < graph->iNumVertices; i++)
    {
        prtOne(graph, i);
    }
   
}



/************************** prtOne *******************************
void prtOne(Graph graph, int iVertex) 
 Purpose:
    Prints the specified airport, the flightNr/szOrigin for flights 
    that have it as a destination (assume a maximum of 8 of them), 
    and prints the flightNr-szDest of flights that have it as an 
    origin.
 Parameters:
    I  Graph graph - a pointer to the graph of flights/airports
    I  int iVertex - the number of the current vertex
 Returns:
    N/A
 Notes:
    1.  This function uses the vertex successor list to print 
        the correct data.
    2.  A warning will be printed if the airport does not exist.
 ****************************************************************/
void prtOne(Graph graph, int iVertex)
{
    // counts of how many successors and predecessors are found.
    int iSuccessorCnt, iPredCnt;
    
    // total number of dot patterns according to example output.
    int totalDots = 8;
    
    if (graph->vertexM[iVertex].bExists != FALSE)
    {
        iPredCnt = prtPredecessors(graph, iVertex);
        totalDots = totalDots - iPredCnt;
        prtDots(totalDots);
        
        // if no successors, print out 1 pattern of dots.
        iSuccessorCnt = prtSuccessors(graph, iVertex);
        if (iSuccessorCnt == 0)
        prtDots(1);
        printf("\n");
    }
}



/********************** prtFlightsByOrigin ************************
 void prtFlightsByOrigin(Graph graph)
 Purpose:
    for each airport, prints out the flights where it is the origin.
 Parameters:
    I  Graph graph - a pointer to the graph.
 Returns:
    Nothing
 Notes:
    Uses the vertex successor list to print correct data.
 ****************************************************************/
void prtFlightsByOrigin(Graph graph)
{
    // local variables to hold flight information 
    int i,j, iDeparture, iArrival, iDuration, iFirst, iZoneChange;
    char szAirport[MAX_TOKEN]; 
    char szFlightNr[MAX_TOKEN];
    char szDest[MAX_TOKEN];
    EdgeNode *p;
    
    for (i = 0; i < graph->iNumVertices; i++)
    {
        
        // represents that we are at the first node in the list
        iFirst = TRUE;
        
        for (p = graph->vertexM[i].successorList; p != NULL; p = p->pNextEdge)
        {
            // copy info to local variables
            strcpy(szAirport, graph->vertexM[i].szAirport);
            strcpy(szFlightNr, p->flight.szFlightNr);
            strcpy(szDest,p->flight.szDest);
            iDeparture = p->flight.iDepTm2400;
            iDuration = p->flight.iDurationMins;
            iZoneChange = p->flight.iZoneChange;
            iArrival = calcArr2400(iDeparture, iDuration, iZoneChange);
    
            // if first node in list, print out with appropriate format
            if (iFirst == TRUE)
                printf("%-s %s %s %4d %4d %4d\n", szAirport, szFlightNr,szDest,iDeparture, iArrival, iDuration );
            
            // if not first node, print out with appropriate format
            if (iFirst == FALSE)
                printf("%6s %s %4d %4d %4d\n", szFlightNr,szDest,iDeparture, iArrival,iDuration );
            
            iFirst = FALSE;   
        }
    }
}
/********************** prtFlightsByDest ************************
 void prtFlightsByDest(Graph graph)
 Purpose:
    for each airport, prints out the flights where it is the destination.
 Parameters:
    I   Graph graph - a pointer to the graph.
 Returns:
    Nothing
 Notes:
    Uses the vertex predecessor list to print correct data.
 ****************************************************************/
void prtFlightsByDest(Graph graph)
{   
    int i, iFirst, iDeparture, iDuration, iZoneChange, iArrival;
    char szAirport[MAX_TOKEN];
    char szFlightNr[MAX_TOKEN];
    char szOrigin[MAX_TOKEN];
    EdgeNode *p;
    
    // iterate through the vertices.
    for (i = 0; i < graph->iNumVertices; i++)
    {
        // we are at the first node in the list at this point.
        iFirst = TRUE;
        
        // iterate through the nodes of the predecessor list.
        for (p = graph->vertexM[i].predecessorList; p != NULL; p = p->pNextEdge)
        {
            // copy info to local variables
            strcpy(szAirport, graph->vertexM[i].szAirport);
            strcpy(szFlightNr, p->flight.szFlightNr);
            strcpy(szOrigin,p->flight.szOrigin);
            iDeparture = p->flight.iDepTm2400;
            iDuration = p->flight.iDurationMins;
            iZoneChange = p->flight.iZoneChange;
            iArrival = calcArr2400(iDeparture, iDuration, iZoneChange);
            
            if (iFirst == TRUE)
                printf("%-s %3s %3s %4d %4d %3d\n", szAirport, szOrigin,szFlightNr, iDeparture, iArrival, iDuration );
            
            if (iFirst == FALSE)
                printf("%7s %3s %4d %4d %3d\n", szOrigin,szFlightNr,iDeparture, iArrival,iDuration );
            
            iFirst = FALSE;
        }
    }
}

/********************** prtPredecessors *************************
int  prtPredecessors(Graph graph, int iVertex)
 Purpose:
    Prints the predecessors for an airport.
 Parameters:
    I  Graph graph - a pointer to the graph of flights/airports
 *  I  int iVertex - number of the current vertex/airport
 Returns:
    N/A
 Notes:
    1.  This function uses the vertex predecessor list to print 
        the correct data.
    2.  A warning will be printed if the airport does not exist.
 ****************************************************************/
int prtPredecessors(Graph g, int iVertex)
{
    char szOrigin[MAX_TOKEN];
    char szFlightNr[MAX_TOKEN];
    char szAirport[MAX_TOKEN];
    EdgeNode *p;
    int i = 0;
    
    // copy the airport only once.
    strcpy(szAirport, g->vertexM[iVertex].szAirport);
    if (g->vertexM[iVertex].bExists != FALSE)
    {
        printf("%d %s ", iVertex, szAirport);
    }

    for (p = g->vertexM[iVertex].predecessorList; p != NULL; p = p->pNextEdge)
    {
        iV = findAirport(g, p->flight.szOrigin);
        if (g->vertexM[iV].bExists == FALSE) 
            continue;
        else if (strcmp(p->flight.szOrigin, g->vertexM[iVertex].szAirport) == 0)
            continue;
     
        // update, should be different each time.
        strcpy(szOrigin,p->flight.szOrigin);
        strcpy(szFlightNr, p->flight.szFlightNr);
        printf("%-s/%s ", szFlightNr, szOrigin);
        i++;
     }
     return i;
}

/********************** prtSuccessors *************************
int prtSuccessors(Graph graph, int iVertex)
 Purpose:
    Prints the successors for an airport.
 Parameters:
    I  Graph graph - a pointer to the graph of flights/airports
 *  I  int iVertex - number of the current vertex/airport
 Returns:
    N/A
 Notes:
    1.  This function uses the vertex successor list to print 
        the correct data. It is used in prtOne function.
 ****************************************************************/
int prtSuccessors(Graph g, int iVertex)
{
    char szDest[MAX_TOKEN];
    char szFlightNr[MAX_TOKEN];
    char szAirport[MAX_TOKEN];
    EdgeNode *p;
    int i = 0;
    
    // copy the airport only once.
    strcpy(szAirport, g->vertexM[iVertex].szAirport);
    
    //printf("%d %s ", iVertex, szAirport);
    for (p = g->vertexM[iVertex].successorList; p != NULL; p = p->pNextEdge)
    {
        iV = findAirport(g, p->flight.szDest);
        if (g->vertexM[iV].bExists == FALSE)
        {
            continue;
        }
        else if (strcmp(p->flight.szDest, g->vertexM[iVertex].szAirport) == 0)
        {
            continue;
        }
        
        // update, should be different each time.
        strcpy(szDest,p->flight.szDest);
        strcpy(szFlightNr, p->flight.szFlightNr);
        printf("%-s-%s ", szFlightNr, szDest);
        i++;
    }
    return i;
}

/**************************** prtDots ****************************
 void prtDots(int iCount)
 Purpose:
    Prints out a dot pattern
 Parameters:
    I int iCount - count of the number of dot patterns to print.
 Returns:
    N/A
 Notes:
    1. This function is used in prtOne to separate predecessors and successors,
 *****************************************************************/
void prtDots(int iCount)
{
    for (iCount; iCount > 0; iCount--)
    {
        printf(".....  ");
    }
}

