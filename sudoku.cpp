#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>
using namespace std;

#include "sudoku.h"

#define CONVERGENCE_COUNT 3 //number of successive cataclysmic mutations converging on the same organism to end the program
#define ELIMINATION_PERCENT 0.01 //remove bottom .01% of population in each generation
#define CATACLYSMIC_RATE 0.20 //.20% chance of mutation per bit in organism DNA during cataclysmic mutation
#define MUTATION_RATE 0.005 //.5% chance of mutation per bit in organism DNA
#define POP_SIZE 100 //number of organisms to generate and build off of
#define DNA_LENGTH 81 //number of items in file we read in CHANGED
#define MAX_GENERATIONS 700000 //if hit 1,000,000 generations, end program


/* Global Variables */
//int CAPACITY; //capacity
int fitnessRuns; //count number of times we run fitness function
int generation; //current generation number
int convergenceCount; //count number of successive times we converge on same best organism
organism best; //current best organism

priority_queue<organism> printPopulation(priority_queue<organism> population)
{
	priority_queue<organism> population2;
    while(population.size() != 0)
    {
        cout << "DNA: ";
        for(int i=0; i<9; i++)
        {
        	for(int j=0; j<9; j++)
        	{
        		cout << population.top().dna[i].newValues[j] << " ";
        	}
        	cout << endl;
        }
        cout << " Fitness: " << population.top().fitness << endl;
        population2.push(population.top());
        population.pop();
    }
    return population2;
}

int squareSum(int rowBeg, int rowEnd, int columnBeg, int columnEnd, vector<row> puzzle)
{
	int sum=0;
	for(int i = rowBeg; i <= rowEnd; i++)//lets check all the 9 SQUARES to assign the square rule fitness
    {
    	for(int j=columnBeg; j<=columnEnd; j++)
    	{
    		sum+=puzzle[i].newValues[j];
    	}
    }
    if(sum!=45)
    		return 1; //add a penalty
    return 0;
}

int fit(organism org)//returns the fitness of that piece of the puzzle //WORK ON THISSSSS 
{
	fitnessRuns++;
	int fit = 27; //that's what will be counting our fitness, the more you have the better off you are
	//27 because there are 27 possible penalties
    int penalty = 0;//the greater the penalty, the worse it is
    int rowSum=0;
    int columnSum=0;


	for(int i = 0; i < 9; i++)//lets check all the 9 ROWS to assign the row rule fitness
    {
    	rowSum=0;
    	for(int j=0; j<9; j++)
    	{
    		rowSum+=org.dna[i].newValues[j];
    	}
    	if(rowSum!=45)
    		penalty++;
    }

    for(int i = 0; i < 9; i++)//lets check all the 9 COLUMNS to assign the column rule fitness
    {
    	columnSum=0;
    	for(int j=0; j<9; j++)
    	{
    		columnSum+=org.dna[j].newValues[i];
    	}
    	if(columnSum!=45)
    		penalty++;
    }


	penalty+=squareSum(0,2,0,2,org.dna);
    penalty+=squareSum(0,2,3,5,org.dna);
    penalty+=squareSum(0,2,6,8,org.dna);
    penalty+=squareSum(3,5,0,2,org.dna);
    penalty+=squareSum(3,5,3,5,org.dna);
    penalty+=squareSum(3,5,6,8,org.dna);
    penalty+=squareSum(6,8,0,2,org.dna);
    penalty+=squareSum(6,8,3,5,org.dna);
    penalty+=squareSum(6,8,6,8,org.dna);

    fit-=penalty;
    //cout << fit << ", " << penalty << endl;
	return fit;
}

bool convergence(priority_queue<organism> population)
{
	//check if all organisms in population are the same DNA
	organism check;
	check.dna = population.top().dna;

	priority_queue<organism> temp;
	while(population.size() != 0)
	{
		if(!(population.top().dna == check.dna))
			return false;
		temp.push(population.top());
		population.pop();
	}
	return true;
}

vector<row> mutate(vector<row> dna, float rate)
{
	for(int i = 0; i < 9; i++)
	{
		for(int j=0; j<9; j++)
		{
			int randNum = (rand() % 1000) / 1000;
			if((randNum < rate) && (dna[i].originalValues[j] == 0))
				dna[i].newValues[j] = ((rand() % 9)+1);

			 // if (dna[i] == '1')
	         //    	dna[i] = '0';
	         //    else
	         //    	dna[i] = '1';
    	}
	}
	return dna;
}

priority_queue<organism> reproduce(priority_queue<organism> population)
{
	int randNum1 = rand() % 100;
	int randNum11 = rand() % 100;

	int randNum2 = rand() % 100;
	int randNum22 = rand() % 100;

	//choose lesser num to pick parent (closer to top of pq and better fitness)
	if (randNum1 > randNum11)
		randNum1 = randNum11; 

	if (randNum2 > randNum22)
		randNum2 = randNum22; 

	//TODO - want to do tournament style pick for parents

	//TODO - change PQ structure to user-sorted stl container to avoid this in future iterations
	priority_queue<organism> temp; 
	organism parent1, parent2;
	int iterator = 0;

	while(population.size() != 0)
	{
		temp.push(population.top());
		if (iterator == randNum1)
		{
			parent1.dna = population.top().dna;
			parent1.fitness = population.top().fitness;
		}
		if (iterator == randNum2)
		{
			parent2.dna = population.top().dna;
			parent2.fitness = population.top().fitness;
		}
		population.pop();
		iterator++;
	}

	//crossover //NOT SURE*********************************************************************************************
	int crossover = (rand() % 81) + 1; //random point from 1 to 99 //SHOULD THIS BE 81??????????
	int rowNum = crossover/9;
	int columnNum = (crossover % 9)-1;

	organism child1, child2;
	child1.dna = parent1.dna;
	for(int i=0; i<rowNum; i++)//row
	{
		for(int j=0; j<columnNum; j++)//column
		{
			child1.dna[i].newValues[j]=parent2.dna[i].newValues[j];
		}
	}
	child2.dna = parent2.dna;
	for(int i=0; i<rowNum; i++)//row
	{
		for(int j=0; j<columnNum; j++)//column
		{
			child2.dna[i].newValues[j]=parent1.dna[i].newValues[j];
		}
	}

	//mutation
	child1.dna = mutate(child1.dna, MUTATION_RATE);
	child2.dna = mutate(child2.dna, MUTATION_RATE);

	//put child organisms through fitness and add to temp
	child1.fitness = fit(child1);
	child2.fitness = fit(child2);
	temp.push(child1);
	temp.push(child2);

	//won't copy last element of temp back to population, this kills the worst organism
	while(temp.size() > 2) 
	{
		population.push(temp.top());
		temp.pop();
	}

	return population;
}

priority_queue<organism> generate(vector<row> &puzzle)
{
	priority_queue<organism> population;
	int randNum;
	organism org;
	srand (time(NULL));	

	for(int h = 0; h < POP_SIZE; h++)
	{
		vector<row> tempPuzzle;
		tempPuzzle = puzzle;
		//cout<< "temp " << endl;
		//printItemVector(tempPuzzle);
		for(int i = 0; i < 9; i++) //go through the whole puzzle (each row)   //pop_size is 81, so the whole puzzle
		{
			//org.dna = "";
			
			for(int j = 0; j < 9; j++) //go through each item in the row    //dna_length is 81
			{
				if(tempPuzzle[i].originalValues[j] == 0)
				{
					tempPuzzle[i].newValues[j] = ((rand() % 9) + 1); //++ because we don't want 0s
					
					//org.dna.append(to_string(randNum));
				}
				else
				{
					tempPuzzle[i].newValues[j] = tempPuzzle[i].originalValues[j];
				}
			}
		}
		//cout << "new one" << endl;
		//printItemVector(tempPuzzle);
		org.dna = tempPuzzle; //may not work because overloading operator!!!!!!!!!!!!!
		org.fitness = fit(org);
		population.push(org);
	}
		
	return population;
}

int printPuzzle(organism org)
{
	//maybe the number of incorrect things, if it is solved or not, just print out the puzzle

	cout << "Original Puzzle:" << endl;
	for(int i=0; i<9; i++)//rows
	{
		for(int j=0; j<9; j++)//item in row
		{
			cout << org.dna[i].originalValues[j] << " ";
		}
		cout << endl;
	}

	cout << "Best Organism Puzzle Solution:" << endl;
	for(int i=0; i<9; i++)//rows
	{
		for(int j=0; j<9; j++)//item in row
		{
			cout << org.dna[i].newValues[j] << " ";
		}
		cout << endl;
	}

	cout << "Best Organism's Fitness: ";
	return org.fitness;
	// string names = "";
	// int cost = 0;

	// for(int i = 0; i < DNA_LENGTH; i++)
	// {	
	// 	//convert char to int
	// 	if((org.dna[i] - '0') == 1)//not right****************************
	// 	{
	// 		cost += puzzle[i].getCost();
	// 		names.append(items[i].getName() + " ");
	// 	}
	// }

	// cout << names << endl;
	// cout << "Cost = ";

	// return cost;

}

int main() //TODO check if passing population back and forth by copy or reference, could try with global population if necessary
{
	clock_t start;
	double duration;
	start = clock();

	fitnessRuns = 0;
	generation = 0;
	convergenceCount = 0;
	//best.dna = "";
	best.fitness = 0;

	priority_queue<organism> population;
	vector <row> puzzle;
	row temp;

	string filename;
	string token;

	int numOfUnknowns=0; //how many 0s in the puzzle
	int count=0;
	//string name, cost, value, i;

	cout << endl << endl;
	//cout <<"Insert file name: ";
	//cin >> filename;
	
	filename = "puzzle1.csv";

	fstream fs;
	fs.open ("input/" + filename);
	if (fs.is_open())
	{
	   string line1;
	   //getline(fs, line1);
	   //stringstream ss(line1);
	   //ss >> CAPACITY;
	   	       
	   while(getline(fs, line1))
	   {
	       istringstream ss(line1);
	       count = 0;
	       
	       while(getline(ss, token, ','))
	       {
	           
	           int token1 = atoi(token.c_str());//make it an integer
	           temp.originalValues[count]=token1;
	           count++;
	           if(token1 == 0)
	           	numOfUnknowns++;
	       }
	       
	       /*
	       int cost1 = atoi(cost.c_str());
	       temp.setCost(cost1);
	       int value1 = atoi(value.c_str());
	       temp.setValue(value1);*/
	       puzzle.push_back(temp);


	   }
	}
	fs.close();

	//printItemVector(puzzle);
	population = generate(puzzle);

	//check if time > 10 minutes, generation > 1,000,000 or converge 3 times on same value
	while((( clock() - start ) / (double) CLOCKS_PER_SEC <= 600) && generation < MAX_GENERATIONS && convergenceCount != 3)
	{
		generation++;
		population = reproduce(population);

		if(population.top().fitness > best.fitness)
		{
			best.dna = population.top().dna;
			best.fitness = population.top().fitness;
			convergenceCount = 0;
		}

		//make sure we don't mess up our current population
		priority_queue<organism> population2 = population;
		if(convergence(population2))
		{
			cout << "hello" << endl;
			if(population.top().dna == best.dna)
			{
				convergenceCount++;
				cout << "CONVERGENCE " << convergenceCount << " *******************" << endl;
			}
			else
			{
				best.dna = population.top().dna;
				best.fitness = population.top().fitness;
				convergenceCount = 1;
			}

			//cataclysmic convergence, add in best and then mutate all
			priority_queue<organism> temp;
			temp.push(best);
			while(population.size() != 0)
			{
				organism mutated;
				mutated.dna = mutate(population.top().dna, CATACLYSMIC_RATE);
				mutated.fitness = fit(mutated);
				temp.push(mutated);
				population.pop();
			}

			//kill bottom organism
			while(temp.size() > 1) 
			{
				population.push(temp.top());
				temp.pop();
			}
		}

	}

	cout << "Time to Run: " << ( clock() - start ) / (double) CLOCKS_PER_SEC << " seconds" << endl;
	cout << "Total Generations: " << generation << endl;
	//TODO create new item vector and print for items instead
	cout << "Best Organism: " << endl;
	cout << printPuzzle(population.top()) << endl;
	cout << "Fitness = " << population.top().fitness << endl;
	cout << "Uses of Fitness Function: " << fitnessRuns << endl;
	cout << endl << endl;

	//printPopulation(population);
}