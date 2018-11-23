#include<iostream>
#include<string>
#include<set>
#include<queue>
#include <boost/lexical_cast.hpp>
#include <sstream>
using namespace std;

int main(){
  string a="10";
  char b = a.at(0);

  int i = boost::lexical_cast<int>(a);

  cout << i+10;

  int num=30;
  ostringstream stri;
  stri << num;

  string c=stri.str();
  cout << c << endl;

  cout << a+',';

  return 0;
}

//brew install boost --c++11
