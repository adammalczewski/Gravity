
struct Col{
  int n1,n2;
  double t;
  double e;       
};

class Heap{
public:
	Heap();
	~Heap();
	Col Take();
	int GiveSize();
	double Top();
	void Add(Col x);
private:
	int size;
	Col heap[1000000];
	void swap(int i,int j);
	void up(int i);
	void down(int i);
	void remove(int i);
};
