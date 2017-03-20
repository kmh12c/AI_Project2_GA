class row
{
private:

public:
	int originalValues[9];
	int newValues[9];
	void operator=(row a);
};



void printItemVector(vector<row> puzzle)
{
    cout << "Original Puzzle:" << endl;
    for (int i=0; i < 9; i++)//num of rows
    {
        for(int j=0; j<9; j++)//items in rows
        {
        	cout << puzzle[i].originalValues[j] << " ";
        }
        cout << endl;
    }
    cout << "Solution Puzzle:" << endl; 
    for (int i=0; i < 9; i++)//num of rows
    {
        

        for(int j=0; j<9; j++)//items in rows
        {
        	cout << puzzle[i].newValues[j] << " ";
        }
        cout << endl;
    }
}

struct organism //TODO turn into class and remove memory
{
    vector<row> dna; //the "solution" puzzle
    int fitness;
    //string solutionDNA;
};

/* overload comparison operators for priority queue to sort organisms by fitness */
bool operator<(const organism & a, const organism & b) {
  return a.fitness < b.fitness;
}

bool operator>(const organism & a, const organism & b) {
  return a.fitness > b.fitness;
}

void row::operator=(row a) {
	  for(int i=0; i<9; i++)
	  {
	  	this->originalValues[i] = a.originalValues[i];
	  }

	  for(int i=0; i<9; i++)
	  {
	  	this->newValues[i] = a.newValues[i];
	  }
	}

bool operator==(const vector<row> & a, const vector<row> & b) {
  for(int i=0; i<9; i++)
  {
  	for(int j=0; j<9; j++)
  	{
  		if(!(a[i].newValues[j] == b[i].newValues[j]))
  			return false;
  	}
  }
  return true;
}
