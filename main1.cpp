#include<iostream>
#include<string>
#include<set>
#include<queue>

using namespace std;

int main(){
  queue<string> myqueue;
  string s="hello";
  string k;

  s="bye";
  myqueue.push(s);
  cout << s;
  cout << s[0];

  string current = myqueue.front();
  if(current.find("h") != string::npos){cout << "si";}
  if(current.size()>1){cout << "Im big";}

  // for(auto& letter: myset){
  //   cout << letter << ",";
  //   if(s.find(letter) != string::npos){
  //     cout << "found";
  //     break;
  //   }
  //   else{cout << "not found";}
  // }



  return 0;
}
