/////////////////////////////////////////////////////////////////////////////////////
// File       : CNF_seq.c
// Language   : C/C++
// Description: SAT Formula (CNF) Generator for Sequential circuits 
// Author     : Sandesh Prabhakar
// Created    : February 8, 2009
// Modified   : August 21, 2026
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

    int count_tim = (count-1)*3 + 1;
    int psuedo_in = count_tim;
    int psuedo_out = 8000; 
    // allocate space for gates data structure
    //gtype = new unsigned char[count];
    gtype = new unsigned char[count_tim]; 
    fanin = new short[count_tim];
    fanout = new short[count_tim];
    levelNum = new int[count];
    faninlist = new int * [count_tim];
    fanoutlist = new int * [count_tim];

    // now read in the circuit
    for (i=1; i<count; i++)
    {
	fscanf(inFile, "%d", &gatenum);
	fscanf(inFile, "%d", &f1);
	fscanf(inFile, "%d", &f2);
	fscanf(inFile, "%d", &f3);

	numgates++;
	gtype[gatenum] = (unsigned char) f1;
        gtype[gatenum+count-1] = (unsigned char) f1;
        gtype[gatenum+2*(count-1)] = (unsigned char) f1; 

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
        fanin[gatenum+count-1] = (int) f3;
        fanin[gatenum+2*(count-1)] = (int) f3;
	// now read in the faninlist
	faninlist[gatenum] = new int[fanin[gatenum]];
        faninlist[gatenum+count-1] = new int[fanin[gatenum+count-1]];
        faninlist[gatenum+2*(count-1)] = new int[fanin[gatenum+2*(count-1)]];
	for (j=0; j<fanin[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);
            if(gtype[f1]!=5)
	    {
	    faninlist[gatenum][j] = (int) f1;
            faninlist[gatenum+count-1][j] = faninlist[gatenum][j]+count-1; 
            faninlist[gatenum+2*(count-1)][j] = faninlist[gatenum][j]+2*(count-1) ;
	    }
            else
	    {
            faninlist[gatenum][j] = psuedo_in;
            faninlist[gatenum+count-1][j] = (int) f1;
            faninlist[gatenum+2*(count-1)][j] = faninlist[gatenum+count-1][j] +count-1;
            psuedo_in++;  
	    }
	}

	for (j=0; j<fanin[gatenum]; j++) // followed by samethings
	    fscanf(inFile, "%d", &junk);

	// read in the fanout list
	fscanf(inFile, "%d", &f1);
	fanout[gatenum] = (int) f1;
        fanout[gatenum+count-1] = (int) f1;
        fanout[gatenum+2*(count-1)] = (int) f1;

	fanoutlist[gatenum] = new int[fanout[gatenum]];
        fanoutlist[gatenum+count-1] = new int[fanout[gatenum+count-1]];
        fanoutlist[gatenum+2*(count-1)] = new int[fanout[gatenum+2*(count-1)]];
	for (j=0; j<fanout[gatenum]; j++)
	{
	    fscanf(inFile, "%d", &f1);

            if(gtype[gatenum]!=5)
	    {
	    fanoutlist[gatenum][j] = (int) f1;
            fanoutlist[gatenum+count-1][j] = fanoutlist[gatenum][j] + count-1;
            fanoutlist[gatenum+2*(count-1)][j] = fanoutlist[gatenum][j] + 2*(count-1);
	    }
            else
	    {
            fanoutlist[gatenum][j] =  (int) f1 + count-1;
            fanoutlist[gatenum+count-1][j] =  fanoutlist[gatenum][j] + count-1;
            fanoutlist[gatenum+2*(count-1)][j] = psuedo_out;
            psuedo_out++;
	    }
            
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

    total_clauses*=3;
    printf("Successfully read in circuit:\n");
    printf("\t%d PIs.\n", numInputs);
    printf("\t%d POs.\n", numOutputs);
    printf("\t%d count_tim.\n", count_tim);
    printf("\t%d total number of variables\n", psuedo_in-1);
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
    fprintf(outFile, "p cnf %d %d\n", psuedo_in-1, total_clauses);

	for(i=1;i<count_tim;i++)
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
                       fprintf(outFile, "%d -%d 0\n",faninlist[i][0], i );
                       fprintf(outFile, "-%d %d 0\n",faninlist[i][0], i );
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


