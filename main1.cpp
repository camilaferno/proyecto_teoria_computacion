#include<iostream>
#include<string>
#include<set>
#include<queue>
using namespace std;

int main(){
  queue<string> myqueue;
  string s="bc?aed??";
  string k;

  // for(int i=0; i<s.size(); i++){
  //   k=s.substr(i, 1);
  //   cout << i << " " << k << endl;
  //
  //
  // }
  //
  // sort(s.begin(), s.end());
  // cout << s << endl;


  string a="A";
  string b="B";
  string z="Z";

  if(z>a){cout << a << "mas grande";}
  else{cout << z << "mas grande";}

  int count =2;
  string tmp="A";
  string nextLetter(1,static_cast<char>(tmp[0] + 1));
  string ab=string nextnextLetter(1,static_cast<char>(nextLetter[0] + count));
  cout << nextLetter << " " << ab;

  // if(s.find("?") != string::npos && s.size()>1){
  //   s.erase(remove(s.begin(), s.end(), '?'), s.end());
  // }
  // cout << s << endl;
  // sort(s.begin(), s.end());
  // cout << s << endl;
  // myqueue.push(s);
  //
  // string current=myqueue.front();
  // cout << current.size() << current;
  //
  // for(int i=0; i<current.size(); i++){
  //   k=current.substr(i, 1);
  //   cout << k << " ";}

  // s="bye";
  // myqueue.push(s);
  // cout << s;
  // cout << s[0];
  //
  // string current = myqueue.front();
  // if(current.find("h") != string::npos){cout << "si";}
  // if(current.size()>1){cout << "Im big";}

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
