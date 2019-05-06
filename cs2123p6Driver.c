#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p6.h"

/******************** getToken **************************************
char * getToken (char *pszInputTxt, char szToken[], int iTokenSize)
Purpose:
    Examines the input text to return the next token.  It also
    returns the position in the text after that token.
Parameters:
    I   char *pszInputTxt       input buffer to be parsed
    O   char szToken[]          Returned token.
    I   int iTokenSize          The size of the token variable.  This is used
                                to prevent overwriting memory.  The size
                                should be the memory size minus 1 (for
                                the zero byte).
Returns:
    Functionally:
         Pointer to the next character following the delimiter after the token.
         NULL - no token found.
    szToken parm - the returned token.  If not found, it will be an
         empty string.
Notes:
    - If the token is larger than the szToken parm, we return a truncated value.
    - If a token isn't found, szToken is set to an empty string
    - This function does not skip over white space occurring prior to the token.
    - If the input buffer pointer is NULL, the function terminates with
      an algorithm error.
**************************************************************************/
char * getToken (char *pszInputTxt, char szToken[], int iTokenSize)
{
    int iDelimPos;                      // found position of delim
    int iCopy;                          // number of characters to copy
    char szDelims[20] = " \n\r";        // delimiters
    szToken[0] = '\0';

    // check for NULL pointer
    if (pszInputTxt == NULL)
        errExit("getToken passed a NULL pointer");

    // Check for no token if at zero byte
    if (*pszInputTxt == '\0')
        return NULL;

    // get the position of the first delim
    iDelimPos = strcspn(pszInputTxt, szDelims);

    // See if the token has no characters before delim
    if (iDelimPos == 0)
        return NULL;

    // see if we have more characters than target token, if so, trunc
    if (iDelimPos > iTokenSize)
        iCopy = iTokenSize;             // truncated size
    else
        iCopy = iDelimPos;

    // copy the token into the target token variable
    memcpy(szToken, pszInputTxt, iCopy);
    szToken[iCopy] = '\0';              // null terminate

    // advance the position
    pszInputTxt += iDelimPos;
    if (*pszInputTxt == '\0')
        return pszInputTxt;
    else
        return pszInputTxt + 1;
}

AltPath altPath;
AltPath newAltPath()
{
    AltPath ap = (AltPath) malloc (sizeof(AltPathImp));
    if (ap == NULL)
        errExit("could not allocate AltPath");
    ap->iAltCnt = 0;
    return ap;
}

/******************** errExit **************************************
    void errExit(char szFmt[], ... )
Purpose:
    Prints an error message defined by the printf-like szFmt and the
    corresponding arguments to that function.  The number of
    arguments after szFmt varies dependent on the format codes in
    szFmt.
    It also exits the program, returning ERR_EXIT.
Parameters:
    I   char szFmt[]            This contains the message to be printed
                                and format codes (just like printf) for
                                values that we want to print.
    I   ...                     A variable-number of additional arguments
                                which correspond to what is needed
                                by the format codes in szFmt.
Returns:
    Returns a program exit return code:  the value of ERR_EXIT.
Notes:
    - Prints "ERROR: " followed by the formatted error message specified
      in szFmt.
    - Prints the file path and file name of the program having the error.
      This is the file that contains this routine.
    - Requires including <stdarg.h>
**************************************************************************/
void errExit(char szFmt[], ... )
{
    va_list args;               // This is the standard C variable argument list type
    va_start(args, szFmt);      // This tells the compiler where the variable arguments
                                // begins.  They begin after szFmt.
    printf("ERROR: ");
    vprintf(szFmt, args);       // vprintf receives a printf format string and  a
                                // va_list argument
    va_end(args);               // let the C environment know we are finished with the
                                // va_list argument
    printf("\n\tEncountered in file %s\n", __FILE__);  // this 2nd arg is filled in by
                                // the pre-compiler
    exit(ERR_EXIT);
}


/******************** readInput **************************************
void readInput()
Purpose:
    Reads commands from stdin
Parameters:
    I Simulation S
Returns:
    Nothing
Notes:
    None
Apt F# Dest Dep  Arr  Dur
SAT S1 IAH  0600 0630  30
    S2 MCO  0800 1130 150
    S3 ATL  1000 1330 150
IAH H1 SAT  1200 1240  40
    H2 ATL  0300 0620 140
    H3 MCO  1400 1710 130
    H4 LAX  1300 1420 200
    H5 PHX  1500 1600 120
PHX P1 DEN  0700 0740  40
    P2 IAH  0800 1050 110
    P3 LAX  0900 0930  90
**************************************************************************/
void readInput(Graph G){
    char szInputBuffer[100];
    char *pszRemainingTxt;
    char token[18];
    while (fgets(szInputBuffer, MAX_LINE_SIZE, stdin) != NULL) {
        if (szInputBuffer[0] == '\n')
            continue;
        printf("%s",  szInputBuffer);
        pszRemainingTxt = getToken(szInputBuffer, token, sizeof(token)-1);
        if(strcmp(token, "*") == 0){
            // NOP
        } else if (strcmp(token, "FLIGHT") == 0){ // Add a new flight
            char szFlightNr[5];
            char szOrigin[5];
            char szDest[5];
            int iDepTm2400;
            int iDurationMins;
            int iZoneChange;
            int iScanCnt;
            iScanCnt = sscanf(pszRemainingTxt, "%s %s %s %d %d %d", szFlightNr, szOrigin, szDest, &iDepTm2400, &iDurationMins, &iZoneChange);

            if(iScanCnt != 6)
                errExit("Malformed data - expected <szflightNr> <szOrigin> <szDest> <iDepTm2400> <iDurationMins> <iZoneChange>");


            // TODO: Test for existing szFlightNr, if encountered, print a warning and continue.

            // Check if airports exist, if not, add it to vertex array
            int iVertexOrigin = findAirport(G, szOrigin);
            int iVertexDest   = findAirport(G, szDest  );

            if(iVertexOrigin == -1){ // Add the origin
                addVertex(G, szOrigin);
            }

            if(iVertexDest == -1){ // Add the destination
                addVertex(G, szDest);
            }

            Flight new;
            new.iDepTm2400 = iDepTm2400;
            new.iDurationMins = iDurationMins;
            new.iZoneChange = iZoneChange;

            strcpy(new.szDest, szDest);
            strcpy(new.szFlightNr, szFlightNr);
            strcpy(new.szOrigin, szOrigin);

            // TODO: Insert ordered by flight nr.
            insertFlight(G, new);

        }else if(strcmp(token, "PRTONE") == 0) { // Print one airport (airport)

            char szApt[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s", szApt);

            if(iScanCnt != 1){
                errExit("Expected airport!");
            }

            int iAirportIndex = findAirport(G, szApt);
            if(iAirportIndex == -1){
                printf(WARN_Specified_Airport_Not_Found, szApt);
            } else {
                // TODO: Print out airport
                printf("Vx Apt Predecessors \t\t\t\t\t\tSuccessors\n");
                prtOne(G, iAirportIndex);
            }
        }else if(strcmp(token, "PRTALL") == 0) { // Print all airports
            printf("Vx Apt Predecessors \t\t\t\t\t\tSuccessors\n");
            prtAll(G);
        }else if(strcmp(token, "PRTFLIGHTBYORIGIN") == 0) { // Print every airport, show where flights leave from.

            printf("%s\n", "Apt F# Dest Dep  Arr  Dur\n"); // Header
            // TODO: Iterate thru airports
            /*
             Print Origin code
             If first, print one space, otherwise print four spaces
             Then print the first entry,
             printf("%s %s %d %d %d\n", f.szFlightNr, f.szDest, f.iDepTm2400, calcArr(f.iDepTm2400, f.iDurationMins, iZoneChange))
             */
            prtFlightsByOrigin(G);
        }else if(strcmp(token, "PRTFLIGHTBYDEST") == 0) { // Print every airport, show where flights arrive to
            
            printf("%s\n", "Apt Orig Fl Dep  Arr  Dur\n");
            prtFlightsByDest(G);
            
        }else if(strcmp(token, "PRTSUCC") == 0) { // Print successors (origin)
            char szApt[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s", szApt);

            if(iScanCnt != 1){
                printf("Error: Expected airport!");
            } else {
                int iAirportIndex = findAirport(G, szApt);
                if(iAirportIndex != -1) {
                    prtTraversal(G, iAirportIndex, 0);
                    setNotVisited(G);
                } else {
                    printf(WARN_Specified_Airport_Not_Found, szApt);
                }
            }
            
            
        }else if(strcmp(token, "PRTCHRON") == 0) {
            
            char szApt[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s", szApt);

            if(iScanCnt != 1){
                printf("Error: Expected airport!\n");
            } else {
                int iAirportIndex = findAirport(G, szApt);
                if (iAirportIndex == -1) {
                    printf(WARN_Specified_Airport_Not_Found, szApt);
                } else {
                    int iIndent = 0;
                    int iPrevArrTm2400 = 0;
                    prtTraversalChron(G, iAirportIndex, iIndent,iPrevArrTm2400 );
                    setNotVisited(G);
                }
            }
        }else if(strcmp(token, "DELETE") == 0) {
            char szApt[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s", szApt);
            if (iScanCnt != 1){
                printf("Expected airport!\n");
            } else{
                int iAirportIndex = findAirport(G, szApt);
                if(iAirportIndex == -1) {
                    printf(WARN_Specified_Airport_Not_Found, szApt);
                } else {
                    //deleteAirport(G, szApt);
                }
            }
        }else if(strcmp(token, "PRTALTS") == 0) {
            char szOrigin[5];
            char szDest[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s %s", szOrigin, szDest);
            if(iScanCnt != 2) {
                printf("PRTALTS expects two airports\n");
            } else {
                if (findAirport(G, szOrigin) == -1){
                    printf(WARN_Specified_Airport_Not_Found, szOrigin);
                } else {
                    if (findAirport(G,szDest) == -1) {
                        printf(WARN_Specified_Airport_Not_Found, szDest);
                    } else {
                        prtAlts(G, findAirport(G, szOrigin), findAirport(G,szDest));
                    }
                }
            }
        }else if(strcmp(token, "MAXSTEPS") == 0) {
            char szOriginApt[5];
            char szDestApt[5];
            
            int iScanCnt = sscanf(pszRemainingTxt,"%s %s", szOriginApt, szDestApt);
            
            if(iScanCnt != 2){
                errExit("Expected airport!");
            }
            int iOriginVertex = findAirport(G, szOriginApt);
            int iDestVertex = findAirport(G, szDestApt);
            int iPrevArrival2400 = 0;
            int iMaxSteps;
            setNotVisited(G);
            iMaxSteps = maxStepsChron(G, iOriginVertex, iDestVertex, iPrevArrival2400);
            setNotVisited(G);
            printf("Maximum chain chron for %s to %s contains %d steps\n", szOriginApt, szDestApt, iMaxSteps);
        }
        else if(strcmp(token, "DELETE") == 0)
        {
            char szApt[5];
            int iScanCnt = sscanf(pszRemainingTxt, "%s", szApt);
            int iAirportIndex = findAirport(G, szApt);
            if(iScanCnt != 1)
            {
                errExit("Expected airport!");
            }
            else if (G->vertexM[iAirportIndex].bExists == FALSE)
            {
                printf(WARN_Specified_Airport_Not_Found, szApt);
            }  
            else
            {
                deleteAirport(G, szApt);
            }
        }
    }
}



/******************** main **************************************
int main(int argc, char** argv)
Purpose:
    Entry point for the program
Parameters:
    I int argc - argument count (unused)
    I char** argv - arguments (unused)
Returns:
    Exit code
Notes:
    Works well, short and sweet.
**************************************************************************/
int main(int argc, char** argv) {
    altPath = newAltPath();
    Graph g = newGraph();
    readInput(g);
    freeGraph(g);
    return (0);
}


