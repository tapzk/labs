#include <iostream>
#include <istream>
#include <fstream>
#include <list>
#include <map>
#include <vector>
// #include <algorithm>

using namespace std;

bool isEm(const string filename){ 
    ifstream file(filename, ios::in | ios::binary); 
    if (!file.is_open()){
        return 0;
    }
    file.seekg(0, ios::end);
    streampos size = file.tellg();
    file.close();
    if (size > 0){
        return 1;
    }
    else{
        return 0;
    }
}

struct diapozon
{             
    char s;   
    int freq; 
    int low;  
    int up;
};

struct SORTI
{
    bool operator()(diapozon l, diapozon r)
    { 
        return l.freq > r.freq;
    }
};

void to_code(ifstream &f, ofstream &g)
{
    int count = 0;
    map<char, int> m;            
    map<char, int>::iterator im; 
    list<diapozon> L;            
    while (!f.eof())             // считаем частоты
    {
        char c = f.get();
        m[c]++;
        count++;
    }
    for (im = m.begin(); im != m.end(); im++)
    {
        diapozon p; // заполняем 
        p.s = im->first;
        p.freq = im->second;
        L.push_back(p); 
    }
    L.sort(SORTI());                            
    L.begin()->up = L.begin()->freq;            // h для самого частого
    L.begin()->low = 0;                         
    list<diapozon>::iterator j = L.begin(), j2; 
    j2 = j;
    j++;
    for (; j != L.end(); j++) // low = up prev
    {
        j->low = j2->up;
        j->up = j->low + j->freq;
        j2++;
    }
    int matrix_size = 0;
    for (im = m.begin(); im != m.end(); im++)
    {

        if (im->second != 0)
            matrix_size += 1;  
    }
    g.write((char *)(&matrix_size), sizeof(matrix_size)); //  размер  таблицы в файл
    for (int z = 0; z < 256; z++)
    { // перебираем все символы
        if (m[char(z)] > 0) // если есть 
        { 
            char c = char(z);
            g.write((char *)(&c), sizeof(c)); // заполняем табличку
            g.write((char *)(&m[char(z)]), sizeof(m[char(z)]));
        }
    }
    f.clear();
    f.seekg(0);                                         
    int l = 0, h = 65535, i = 0, delitel = L.back().up; 
    int F_q = (h + 1) / 4, Half = F_q * 2, T_q = F_q * 3, bits_to_follow = 0;
    char tx = 0;
    count = 0;
    while (!f.eof())
    { 
        char c = f.get();
        i++;                                   // считываем символ и идёт по листу
        for (j = L.begin(); j != L.end(); j++) 
        {
            if (c == j->s)
                break; // если нашли выходим
        }
        if (c != j->s) // иначе ошибка
        {
            cout << "oshibka" << endl;
            break;
        }
        int l2 = l;                                  
        l = l + j->low * (h - l + 1) / delitel;     
        h = l2 + j->up * (h - l2 + 1) / delitel - 1; 
        for (;;)                                     
        {
            if (h < Half)
            { 
                count++;
                if (count == 8)
                {
                    count = 0;
                    g << tx; 
                    tx = 0;  
                }
                for (; bits_to_follow > 0; bits_to_follow--)
                { 
                    tx = tx | (1 << (7 - count));
                    count++;
                    if (count == 8)
                    {
                        count = 0;
                        g << tx;
                        tx = 0;
                    }
                }
            }
            else if (l >= Half)
            {                                 
                tx = tx | (1 << (7 - count)); 
                count++;
                if (count == 8)
                {
                    count = 0;
                    g << tx; 
                    tx = 0;
                }
                for (; bits_to_follow > 0; bits_to_follow--)
                { 
                    count++;
                    if (count == 8)
                    {
                        count = 0;
                        g << tx;
                        tx = 0;
                    }
                }
                l -= Half; 
                h -= Half;
            }
            else if ((l >= F_q) && (h < T_q))
            {                     
                bits_to_follow++; 
                l -= F_q;         
                h -= F_q;
            }
            else
                break;
            l += l; 
            h += h + 1;
        }
    }
    g << tx; 
    f.clear();
    f.seekg(0, std::ios::end);
    g.seekp(0, std::ios::end);
    f.close();
    g.close();
}

bool decode(ifstream &fg, ofstream &gf)
{
    if (!fg.is_open())
    {
        return false; 
    }
    int count = 0;                    
    int x1, x2;                     
    char s;                           
    map<char, int> m;                 
    map<char, int>::iterator im;      
    fg.read((char *)&x1, sizeof(x1)); 
    while (x1 > 0)
    {                                  
        fg.read((char *)&s, sizeof(s)); 
        fg.read((char *)&x2, sizeof(x2));
        x1 -= 1;  
        m[s] = x2; 
    }
    list<diapozon> L; 
    for (im = m.begin(); im != m.end(); im++)
    {
        diapozon p; 
        p.s = im->first;
        p.freq = im->second;
        L.push_back(p); // добавляем ее в список
    }
    L.sort(SORTI());                 
    L.begin()->up = L.begin()->freq; 
    L.begin()->low = 0;              
    list<diapozon>::iterator j = L.begin(), j2;
    j2 = j;
    j++;
    for (; j != L.end(); j++)
    {
        j->low = j2->up; 
        j->up = j->low + j->freq;
        j2++;
    }
    count = 0;
    int l = 0, h = 65535, delitel = L.back().up;
    int F_q = (h + 1) / 4, Half = F_q * 2, T_q = F_q * 3; 
    int value = (fg.get() << 8) | fg.get();               
    char symbl = fg.get();                                
    while (!fg.eof())
    {                                                             
        int freq = ((value - l + 1) * delitel - 1) / (h - l + 1); 
        for (j = L.begin(); j->up <= freq; j++)
            ;                                          
        int l2 = l;                                    
        l = l + (j->low) * (h - l + 1) / delitel;      // new low
        h = l2 + (j->up) * (h - l2 + 1) / delitel - 1; // new up
        for (;;)
        { 
            if (h < Half)
                ; 
            else if (l >= Half)
            { 
                l -= Half;
                h -= Half;     // меняем границы
                value -= Half; 
            }
            else if ((l >= F_q) && (h < T_q))
            { 
                l -= F_q;
                h -= F_q;
                value -= F_q; 
            }
            else
                break;
            l += l; 
            h += h + 1;
            value += value + (((short)symbl >> (7 - count)) & 1);
            count++; 
            if (count == 8)
            {
                symbl = fg.get();
                count = 0;
            }
        }
        gf << j->s; 
    }
    fg.close();
    gf.close(); 
    return true;
}

int main()
{   
    ifstream f("1.txt", ios::out | ios::binary);
    ofstream g("code.txt", ios::out | ios::binary);
    cout<<"1 code 2 decode 0 stop"<<endl;
    int mode = 1;
    while(mode != 0){
        cin>>mode;
        if (mode == 1){
            to_code(f, g);
            cout << "code ok" << endl;
            ifstream fg("code.txt", ios::out | ios::binary);
            ofstream gf("output.txt", ios::out | ios::binary);
        }   
        if(mode == 2 && isEm("code.txt")){
        ifstream fg("code.txt", ios::out | ios::binary);
        ofstream gf("output.txt", ios::out | ios::binary);
            if (decode(fg, gf)){
            cout << "decode ok" << endl;
        }
        else{   
            cout << "not decode" << endl;
            }
        }
    }
    return 0;   
}