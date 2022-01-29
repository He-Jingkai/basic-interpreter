#include "exp.h"

using namespace std;

string insert_tab(string a){
    int i=0;
    while(i<(int)a.size())
    {
        if(a[i]=='\n'){
            a.insert(a.begin()+i+1,'\t');
            i+=2;
        }
        else i++;
    }
    return a;
}
