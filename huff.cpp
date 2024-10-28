#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
using namespace std;

class Node
{
	public:
	 int a; // частота
	 char c; // символ
	 Node *left, *right; // указатели туда сюда
	 
	 Node(){left=right=NULL;} // указатель по умолч

	 Node(Node *L, Node *R) 
	 {  left =  L;
	    right = R;
	    a = L->a + R->a;  } // сумма частот
};


struct MyCompare
{
    bool operator()(const Node* l, const Node* r) const { return l->a < r->a; } // сравнение чистот для дальнейшей сортировки
};


vector<bool> code;             // вектор для текущего кода символа   
map<char,vector<bool> > table;    //таблица хафмана

void BuildTable(Node *root) // фунцкия для прохода по дереву создания кода для символа
{	
    if (root->left!=NULL) 
                      { code.push_back(0);
                      BuildTable(root->left);}
     
    if (root->right!=NULL)
                       { code.push_back(1);
                       BuildTable(root->right);}
   
    if (root->left==NULL && root->right==NULL) table[root->c]=code;     // сохраняем
    
    code.pop_back();
}


int main (int argc, char *argv[])
{
////// считаем частоты символов	
	ifstream f("1.txt", ios::out | ios::binary);
	
	map<char,int> m;
	
	while (!f.eof())
	{ char c = f.get(); 
	   m[c]++;}

  
////// записываем начальные узлы в список list
	     	     
   list<Node*> t;
   for( map<char,int>::iterator itr=m.begin(); itr!=m.end(); ++itr)
   {  
      Node *p = new Node;
      p->c = itr->first;
      p->a = itr->second;  
      t.push_back(p);      }	
	

//////  создаем дерево		

  while (t.size()!=1)
  {  
     t.sort(MyCompare()); //сортируем по чистоте
    
     Node *SonL = t.front(); //берем два наименьших
     t.pop_front();
     Node *SonR = t.front(); 
     t.pop_front();
     
     Node *parent = new Node(SonL,SonR); // создаем родительский узел
     t.push_back(parent); // добавляем обратно в список пока не схлопнемся до корня 

  }
	
	Node *root = t.front();   //root - указатель на вершину дерева

////// создаем пары 'символ-код':			

   	BuildTable(root);   
		
////// Выводим коды в файл output.txt

    f.clear(); f.seekg(0); // перемещаем указатель снова в начало файла
    
	ofstream g("output.txt", ios::out | ios::binary);
    	
    int count=0; char buf=0;
    while (!f.eof())
    { char c = f.get();
	  vector<bool> x = table[c];
	  for(int n=0; n<x.size(); n++)
	   {buf = buf | x[n]<<(7-count);   
	    count++;   
	    if (count==8) { count=0;   g<<buf; buf=0; } 
       }
    }

    f.close();
	g.close(); 
	
///// считывание из файла output.txt и преобразование обратно

	ifstream F("output.txt", ios::in | ios::binary);

	setlocale(LC_ALL,"Russian"); // чтоб русские символы отображались в командной строке
	
	Node *p = root;
	count=0; char byte; 
	byte = F.get();
	while(!F.eof())
	{   bool b = byte & (1 << (7-count) ) ; 
		if (b) p=p->right; else p=p->left; //идём по дереву
		if (p->left==NULL && p->right==NULL) {cout<<p->c; p=root;}  // если лист то выводим чар
		count++;
		if (count==8) {count=0; byte = F.get();} // читаем следущий байт
	}
	
	F.close();	

	return 0;
}