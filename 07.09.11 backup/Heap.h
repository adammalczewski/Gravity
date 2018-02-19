
struct Col{
  int type1,type2;  //-1 = border  (n: 0-left,1-right,2-top,3-bottom)
  int n1,n2;
  double t;       
};

class Heap{
public:
	Heap();
	~Heap();
	Col Take();
	int GiveSize();
	void Add(Col x);
private:
	int size;
	Col heap[1000000];
	void swap(int i,int j);
	void up(int i);
	void down(int i);
	void remove(int i);
};
