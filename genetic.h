class Item 
{
    private:
        string name;
        int cost;
        int value;
        
    public:
        void setName (string x) {name = x;}
        void setCost (int x) {cost = x;}
        void setValue(int x) {value = x;}
        string getName() {return name;}
        int getCost() {return cost;}
        int getValue() {return value;}    
};

void printItemVector(vector<Item> items)
{
    for (int i=0; i < items.size(); i++)
    {
        cout << "Name: " << items[i].getName() << " Cost:" << items[i].getCost() << " Value: " << items[i].getValue() << endl;
    }
}

struct organism //TODO turn into class and remove memory
{
    string dna;
    int fitness;
};

/* overload comparison operators for priority queue to sort organisms by fitness */
bool operator<(const organism & a, const organism & b) {
  return a.fitness < b.fitness;
}

bool operator>(const organism & a, const organism & b) {
  return a.fitness > b.fitness;
}