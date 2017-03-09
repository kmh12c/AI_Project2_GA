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

#include "genetic.h"

#define CONVERGENCE_COUNT 3 //number of successive cataclysmic mutations converging on the same organism to end the program
#define ELIMINATION_PERCENT 0.01 //remove bottom .01% of population in each generation
#define CATACLYSMIC_RATE 0.20 //.20% chance of mutation per bit in organism DNA during cataclysmic mutation
#define MUTATION_RATE 0.005 //.5% chance of mutation per bit in organism DNA
#define POP_SIZE 100 //number of organisms to generate and build off of
#define DNA_LENGTH 100 //number of items in file we read in
#define MAX_GENERATIONS 10000 //if hit 1,000,000 generations, end program

/* Global Variables */
int CAPACITY; //capacity
int fitnessRuns; //count number of times we run fitness function
int generation; //current generation number
int convergenceCount; //count number of successive times we converge on same best organism
organism best; //current best organism

priority_queue<organism> printPopulation(priority_queue<organism> population)
{
	priority_queue<organism> population2;
    while(population.size() != 0)
    {
        cout << "DNA: " << population.top().dna << " Fitness: " << population.top().fitness << endl;
        population2.push(population.top());
        population.pop();
    }
    return population2;
}

int fit(organism org, vector<Item> items)
{
	fitnessRuns++;

	int cost = 0, value = 0;

	for(int i = 0; i < DNA_LENGTH; i++)
	{	
		//convert char to int
		if((org.dna[i] - '0') == 1)
		{
			cost += items[i].getCost();
			value += items[i].getValue();
		}
	}
	
	if(cost > CAPACITY)
		return 0;
	return value;
}

bool convergence(priority_queue<organism> population)
{
	//check if all organisms in population are the same DNA
	organism check;
	check.dna = population.top().dna;

	priority_queue<organism> temp;
	while(population.size() != 0)
	{
		if(population.top().dna != check.dna)
			return false;
		temp.push(population.top());
		population.pop();
	}
	return true;
}

string mutate(string dna, float rate)
{
	for(int i = 0; i < dna.size(); i++)
	{
		int randNum = (rand() % 1000) / 1000;
		if (randNum < rate)
		{
        	if (dna[i] == '1')
            	dna[i] = '0';
            else
            	dna[i] = '1';
        }
	}
	return dna;
}

priority_queue<organism> reproduce(priority_queue<organism> population, vector<Item> items)
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

	//crossover
	int crossover = rand() % 99 + 1; //random point from 1 to 99
	organism child1, child2;
	child1.dna = parent1.dna.substr(0,crossover);
	child1.dna.append(parent2.dna.substr(crossover));
	child2.dna = parent2.dna.substr(0,crossover);
	child2.dna.append(parent1.dna.substr(crossover));

	//mutation
	child1.dna = mutate(child1.dna, MUTATION_RATE);
	child2.dna = mutate(child2.dna, MUTATION_RATE);

	//put child organisms through fitness and add to temp
	child1.fitness = fit(child1, items);
	child2.fitness = fit(child2, items);
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

priority_queue<organism> generate(vector<Item> items)
{
	priority_queue<organism> population;
	int randNum;
	organism org;
	srand (time(NULL));	

	for(int i = 0; i < POP_SIZE; i++)
	{
		org.dna = "";
		
		for(int j = 0; j < DNA_LENGTH; j++)
		{
			randNum = rand() % 2;
			org.dna.append(to_string(randNum));
		}
		
		org.fitness = fit(org, items);
		population.push(org);
	}
		

	return population;
}

int printItems(organism org, vector<Item> items)
{
	string names = "";
	int cost = 0;

	for(int i = 0; i < DNA_LENGTH; i++)
	{	
		//convert char to int
		if((org.dna[i] - '0') == 1)
		{
			cost += items[i].getCost();
			names.append(items[i].getName() + " ");
		}
	}

	cout << names << endl;
	cout << "Cost = ";

	return cost;
}

int main() //TODO check if passing population back and forth by copy or reference, could try with global population if necessary
{
	clock_t start;
	double duration;
	start = clock();

	fitnessRuns = 0;
	generation = 0;
	convergenceCount = 0;
	best.dna = "";
	best.fitness = 0;

	priority_queue<organism> population;
	vector <Item> items;
	Item temp;

	string filename;
	string token;

	int capacity, count;
	string name, cost, value, i;

	cout << endl << endl;
	//cout <<"Insert file name: ";
	//cin >> filename;
	
	filename = "F2.csv";

	fstream fs;
	fs.open ("input/" + filename);
	if (fs.is_open())
	{
	   string line1;
	   getline(fs, line1);
	   stringstream ss(line1);
	   ss >> CAPACITY;
	   	       
	   while(getline(fs, line1))
	   {
	       istringstream ss(line1);
	       count = 0;
	       
	       while(getline(ss, token, ','))
	       {
	           
	           if(count == 0)
	               name = token;
	           else if(count == 1)
	               cost = token;
	           else
	               value = token;
	           count++;
	       }
	       
	       temp.setName(name);
	       int cost1 = atoi(cost.c_str());
	       temp.setCost(cost1);
	       int value1 = atoi(value.c_str());
	       temp.setValue(value1);

	       items.push_back(temp);
	   }
	}
	fs.close();

	cout << "Capacity: " << CAPACITY << endl << endl << endl;

	population = generate(items);

	//check if time > 10 minutes, generation > 1,000,000 or converge 3 times on same value
	while((( clock() - start ) / (double) CLOCKS_PER_SEC <= 600) && generation < MAX_GENERATIONS && convergenceCount != 3)
	{
		generation++;
		population = reproduce(population,items);

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
				mutated.fitness = fit(mutated, items);
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
	cout << "Best Organism: Items = " << printItems(population.top(), items) << ", Value = " << population.top().fitness << endl;
	cout << "Uses of Fitness Function: " << fitnessRuns << endl;
	cout << endl << endl;

	//printPopulation(population);
}