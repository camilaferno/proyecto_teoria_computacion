#include<iostream>
#include<string>
#include<set>
#include<queue>
#include <sstream>
using namespace std;

void sort_string(string &stri){
  vector<int> vec;
  string tmp;
  stringstream stream(stri);
  while(getline(stream, tmp, ',')){
    vec.push_back(stoi(tmp));
  }
  sort(vec.begin(), vec.end());

  stri.clear();
  stri = to_string(*vec.begin());

  for (std::vector<int>::iterator it=vec.begin()+1; it!=vec.end(); ++it){
    stri += ',' + to_string(*it);
  }
}

int main(){
  string a="45";
  char b = a.at(0);

  int num=30;
  ostringstream stri;
  stri << num;

  string c=stri.str();
  cout << c << endl;

  vector<string> vec;

  string str = "0x0002,A5651QPR87GBZ094RTF52,D,A,000001,ABC ,10000.00 , EOT";
    string word;
    stringstream stream(str);
    while( getline(stream, word, ',') )
        vec.push_back(word);

  string x="2,13";

  sort_string(x);
  cout << x;

  return 0;
}

//brew install boost --c++11
