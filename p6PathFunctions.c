/*
    new functions for pgm6. deleteAirport will be in the flightdata function.
 */

#include "cs2123p6.h"
#include <stdio.h>
#include <string.h>

extern AltPath altPath;

/******************** prtAlts **************************************
void prtAlts(Graph graph, int iOriginVertex, int iDestVertex)
Purpose:
    Print all acceptable alternative routes to a destination
Parameters:
    I Graph graph - Graph we're working with
    I int iOriginVertex - Where we start
    I int iDestVertex - Where we're going
Returns:
    Nothinge
Notes:

**************************************************************************/
void prtAlts(Graph graph, int iOriginVertex, int iDestVertex)
{
    char * szOrigin = graph->vertexM[iOriginVertex].szAirport;
    char * szDest = graph->vertexM[iDestVertex].szAirport;
    
    altPath->iAltCnt = 0;
    Path p;
    p.iStepCnt=0;
    p.iTotalDuration=0;
    setNotVisited(graph);
    determinePaths(graph, iOriginVertex, iDestVertex, 0, p, -1);
    printf("Alternatives for flights from %s to %s\n", szOrigin, szDest);
    printf("Alt TDur             Org Dst Fl Dep - Arr Dura\n");
    int i;
    for(i=0;i<altPath->iAltCnt;i++){
        Path n = altPath->altPathM[i];
        int o;
        EdgeNode * E = n.stepM[n.iStepCnt-1];
        int a = calcArr2400(E->flight.iDepTm2400, E->flight.iDurationMins, E->flight.iZoneChange) - (n.stepM[0])->flight.iDepTm2400;
        //printf("%d\n", a);
        int h = (a-(a%100))/100;
        int m = a%100;
        printf("  %d %2d Hours %2d mins\n", i+1, h, m);
        for(o=0;o<n.iStepCnt;o++){
            //
            EdgeNode * v = n.stepM[o];
            printf("                     %s %s %s %04d-%04d %4d\n", v->flight.szOrigin, v->flight.szDest, v->flight.szFlightNr, v->flight.iDepTm2400, calcArr2400(v->flight.iDepTm2400, v->flight.iDurationMins, v->flight.iZoneChange), v->flight.iDurationMins);
        }
    }
    setNotVisited(graph);
    // do stuff
}

/*********************** maxStepsChron ***************************
 
 Purpose:
    finds the maximum number of steps in a path.
 Parameters:
    I/O    Graph graph - graph of the airports as a double adjacency list.
    I      int iVertex  - origin vertex/ successor vertex in subsequent calls.
    I      int iDestVertex - destination of the vertex we are searching for.
    I      int iPrevArrTm2400  -   previous arrival time.
 Returns:
    1. returns 1 if the destination vertex is a valid successor.
    2. returns 0 if the vertex has been visited or a path is found.
 Notes:
    function is recursive, setNotVisited(graph) is used after calling to reset bVisited.
 
 *****************************************************************/
int maxStepsChron(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
{
    
    if (graph->vertexM[iVertex].bVisited == TRUE)
        return 0;
    graph->vertexM[iVertex].bVisited = TRUE;
    
    if (iVertex == iDestVertex)
    {
        return 0;
    }
        
    
    int iSteps = 0;
    int iMax = 0;
    EdgeNode *e;
    for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
    {
        //if valid
        if (e->flight.iDepTm2400 > (iPrevArrTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS))
        {
            int nextArrival = calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange);
            iSteps = 1 + maxStepsChron(graph, e->iDestVertex, iDestVertex, nextArrival);
        }
 
        if (iMax < iSteps)
            iMax = iSteps;
        
        graph->vertexM[iVertex].bVisited = FALSE;
    }
    return iMax;
}

/******************** determinePaths **************************************
void determinePaths(Graph graph, int iVertex, int iDestVertex, int iCurStep, Path path, int iPrevArrTm2400)
Purpose:
    Creates paths in altPath (used by prtAlts)
Parameters:
    I Graph graph - Graph we're working with
    I int iVertex - Where we start
    I int iDestVertex - Where we're going
    I Path path - path we're manipulating
    I int iPrevArrTm2400 - last flight's arrival time
Returns:
    Nothing
Notes:
	setNotVisited(graph) before & after use
**************************************************************************/
void determinePaths(Graph graph, int iVertex, int iDestVertex, int iCurStep, Path path, int iPrevArrTm2400)
{
    Vertex V = graph->vertexM[iVertex];
    Vertex D = graph->vertexM[iDestVertex];
    EdgeNode *E;
    V.bVisited = TRUE;
    for(E = V.successorList; E != NULL; E = E->pNextEdge){
        Flight Me = E->flight;
        int iDepartAt = Me.iDepTm2400;
        int iArriveAt = calcArr2400(iDepartAt, Me.iDurationMins, Me.iZoneChange);
        if(strcmp(E->flight.szDest, D.szAirport) == 0 && iDepartAt > iPrevArrTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS) { // found an end
            path.stepM[iCurStep] = E;
            path.iStepCnt = iCurStep+1;
            path.iTotalDuration = calcArr2400(E->flight.iDepTm2400, E->flight.iDurationMins, E->flight.iZoneChange) - path.stepM[0]->flight.iDepTm2400;
            altPath->altPathM[altPath->iAltCnt] = path;
            altPath->iAltCnt++;
        } else {
            if(graph->vertexM[E->iDestVertex].bVisited == FALSE) {
                graph->vertexM[E->iOriginVertex].bVisited = TRUE;
                if(iCurStep == 0) {
                    path.stepM[iCurStep] = E;
                    path.iStepCnt = iCurStep;
                    determinePaths( graph, E->iDestVertex, iDestVertex, iCurStep + 1, path, iArriveAt);
                } else {
                    if((iPrevArrTm2400 == -1) || iDepartAt > iPrevArrTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS) { // 
                        path.stepM[iCurStep] = E;
                        path.iStepCnt = iCurStep;
                        determinePaths( graph, E->iDestVertex, iDestVertex, iCurStep + 1, path, iArriveAt);

                    }

                }
                graph->vertexM[E->iOriginVertex].bVisited = FALSE;
            }
        }
        
    }
}
