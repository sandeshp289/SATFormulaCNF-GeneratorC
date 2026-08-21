/////////////////////////////////////////////////////////////////////////////////////
// File       : CNF_combo.c
// Language   : C/C++
// Description: SAT Formula (CNF) Generator for Combinational circuits 
// Author     : Sandesh Prabhakar
// Created    : December 8, 2007
// Modified   : August 15, 2026
// Version    : 1.0
//
// Copyright (c) 2026 Sandesh Prabhakar
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
   G_JUNK,         /* 0 */
   G_INPUT,        /* 1 */
   G_OUTPUT,       /* 2 */
   G_XOR,          /* 3 */
   G_XNOR,         /* 4 */
   G_DFF,          /* 5 */
   G_AND,          /* 6 */
   G_NAND,         /* 7 */
   G_OR,           /* 8 */
   G_NOR,          /* 9 */
   G_NOT,          /* 10 */
   G_BUF,          /* 11 */
};

////////////////////////////////////////////////////////////////////////
// circuit class
////////////////////////////////////////////////////////////////////////

class circuit
{
    // circuit information
    int numgates;       // total number of gates (faulty included)
    int maxlevels;      // number of levels in gate level ckt
    int numInputs;	// number of inputs
    int numOutputs;	// number of outputs
    unsigned char *gtype;	// gate type
    short *fanin;		// number of fanins of gate
    short *fanout;		// number of fanouts of gate
    int *levelNum;		// level number of gate
    int **faninlist;		// fanin list of gate
    int **fanoutlist;		// fanout list of gate

    // you may wish to add more private variables here

public:
    circuit(char *);            // constructor

    // you may wish to add more function declarations (or public variables) here
};

////////////////////////////////////////////////////////////////////////
// constructor: reads in the *.lev file and builds basic data structure
// 		for the gate-level ckt
////////////////////////////////////////////////////////////////////////
circuit::circuit(char *cktName)
{
    FILE *inFile, *outFile;
    char fName[40];
    int i, j, count;
    char c;
    int gatenum, junk, total_clauses;
    int f1, f2, f3;

    strcpy(fName, cktName);
    strcat(fName, ".lev");
    inFile = fopen(fName, "r");
    if (inFile == NULL)
    {
	fprintf(stderr, "Can't open .lev file\n");
	exit(-1);
    }

    numgates = maxlevels = 0;
    numInputs = numOutputs = 0;
    total_clauses=0;

    fscanf(inFile, "%d", &count);	// number of gates
    fscanf(inFile, "%d", &junk);	// skip the second line

    // allocate space for gates data structure
    gtype = new unsigned char[count];
    fanin = new short[count];
    fanout = new short[count];
    levelNum = new int[count];
    faninlist = new int * [count];
    fanoutlist = new int * [count];

    // now read in the circuit
    for (i=1; i<count; i++)
    {
	fscanf(inFile, "%d", &gatenum);
	fscanf(inFile, "%d", &f1);
	fscanf(inFile, "%d", &f2);
	fscanf(inFile, "%d", &f3);

	numgates++;
	gtype[gatenum] = (unsigned char) f1;
	if (gtype[gatenum] > 13)
	    printf("gate %d is an unimplemented gate type\n", gatenum);
	else if (gtype[gatenum] == G_INPUT)
	    numInputs++;
	else if (gtype[gatenum] == G_OUTPUT)
	    numOutputs++;

	f2 = (int) f2;
	levelNum[gatenum] = f2;

	if (f2 >= (maxlevels))
	    maxlevels = f2 + 5;

	fanin[gatenum] = (int) f3;
	// now read in the faninlist
	faninlist[gatenum] = new int[fanin[gatenum]];
	for (j=0; j<fanin[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);
	    faninlist[gatenum][j] = (int) f1;
	}

	for (j=0; j<fanin[gatenum]; j++) // followed by samethings
	    fscanf(inFile, "%d", &junk);

	// read in the fanout list
	fscanf(inFile, "%d", &f1);
	fanout[gatenum] = (int) f1;

	fanoutlist[gatenum] = new int[fanout[gatenum]];
	for (j=0; j<fanout[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);
	    fanoutlist[gatenum][j] = (int) f1;
	}

        switch(gtype[gatenum])
	{
		case 0:
                case 1:
                       break;
                case 3:
                case 4:
                       total_clauses+=4;
                       break;
                case 5:
                       break;
                case 2:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
		case 11: 
                       total_clauses+=(fanin[gatenum]+1);
                       break;
		default: 
                       break;
	}


	// skip till end of line
	while ((c = getc(inFile)) != '\n' && c != EOF)
	    ;
    }	// for (i...)
    fclose(inFile);

    printf("Successfully read in circuit:\n");
    printf("\t%d PIs.\n", numInputs);
    printf("\t%d POs.\n", numOutputs);
    printf("\t%d total number of gates\n", numgates);
    printf("\t%d levels in the circuit.\n", maxlevels / 5);
    printf("\t%d Total number of clauses.\n",total_clauses);

    strcpy(fName, cktName);
    strcat(fName, ".cnf");
    outFile = fopen(fName, "w");
    if (outFile == NULL)
    {
	fprintf(stderr, "Can't open .cnf file\n");
	exit(-1);
    }

    fprintf(outFile, "c FILE:%s.cnf\n", cktName);
    fprintf(outFile, "p cnf %d %d\n", numgates, total_clauses);

	for(i=1;i<=numgates;i++)
	{
   		switch(gtype[i])
		{
 		case 0:
                case 1:
                       break;
                case 2:
                       fprintf(outFile, "%d -%d 0\n",faninlist[i][0], i );
                       fprintf(outFile, "-%d %d 0\n",faninlist[i][0], i );  
                       break;
                //XOR
                case 3:
                       fprintf(outFile, "%d %d -%d 0\n",faninlist[i][0], faninlist[i][1], i ); 
                       fprintf(outFile, "-%d -%d -%d 0\n",faninlist[i][0], faninlist[i][1], i );
                       fprintf(outFile, "-%d %d %d 0\n",faninlist[i][0], faninlist[i][1], i );
                       fprintf(outFile, "%d -%d %d 0\n",faninlist[i][0], faninlist[i][1], i );
                       break;
                //XNOR
                case 4:
                       fprintf(outFile, "%d %d %d 0\n",faninlist[i][0], faninlist[i][1], i ); 
                       fprintf(outFile, "-%d -%d %d 0\n",faninlist[i][0], faninlist[i][1], i );
                       fprintf(outFile, "-%d %d -%d 0\n",faninlist[i][0], faninlist[i][1], i );
                       fprintf(outFile, "%d -%d -%d 0\n",faninlist[i][0], faninlist[i][1], i );
                       break;
                //DFF
                case 5:
                       break;
                //AND
                case 6:
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile,"%d -%d 0\n",faninlist[i][j], i );
		       }
                       fprintf(outFile,"%d",i ); 
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, " -%d",faninlist[i][j]);
		       }
                       fprintf(outFile, " 0\n"); 
                       break;
                //NAND
                case 7:
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, "%d %d 0\n",faninlist[i][j], i );
		       }
                       fprintf(outFile, "-%d",i ); 
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, " -%d",faninlist[i][j]);
		       }
                       fprintf(outFile, " 0\n");  
                       break;
                //OR 
                case 8:
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, "-%d %d 0\n",faninlist[i][j], i );
		       }
                       fprintf(outFile, "-%d",i ); 
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, " %d",faninlist[i][j]);
		       }
                       fprintf(outFile, " 0\n"); 
                       break;
                //NOR
                case 9:
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, "-%d -%d 0\n",faninlist[i][j], i );
		       }
                       fprintf(outFile, "%d",i ); 
                       for(j=0;j<fanin[i];j++)
		       {
                       fprintf(outFile, " %d",faninlist[i][j]);
		       }
                       fprintf(outFile, " 0\n"); 
                       break;
                //NOT
                case 10:
                       fprintf(outFile, "%d %d 0\n",faninlist[i][0], i );
                       fprintf(outFile, "-%d -%d 0\n",faninlist[i][0], i );
                       break;
                //BUF
		case 11: 
                       fprintf(outFile, "%d -%d 0\n",faninlist[i][0], i );
                       fprintf(outFile, "-%d %d 0\n",faninlist[i][0], i );
                       break;
		default: 
                       break;
		}

	}

}

////////////////////////////////////////////////////////////////////////
// other member functions here
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// main starts here
////////////////////////////////////////////////////////////////////////
main(int argc, char *argv[])
{
    char cktName[20];
    circuit *ckt;
    int vecWidth, poNum;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <ckt>\n", argv[0]);
        exit(-1);
    }

    strcpy(cktName, argv[1]);

    // read in the circuit and build data structure
    ckt = new circuit(cktName);

    // read in a number
   // printf("Please enter a primary input number:\n");
   // scanf("%d", &poNum);

    // NOW ADD YOUR FUNCTION HERE
}


