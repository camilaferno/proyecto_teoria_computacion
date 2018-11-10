#include<iostream>
#include<string>
#include<set>
#include<queue>
using namespace std;

void union(string &newNode1, string &newNode2, string current){
  for(int i=0; i<current.size(); i++){
    string character = current.substr(i, 1);
    for(ni=nodes.begin();ni!=nodes.end();++ni){
    	if(character==(*ni)->getNdata()){
    		for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ei++){
    			if((*ei)->getEdata()==0){
    				newNode1+=(*ei)->nodes[1]->getNdata();
    			}
    			if((*ei)->getEdata()==1){
    				newNode2+=(*ei)->nodes[1]->getNdata();
    			}
    		}
    	}
    }
  }
}

int main(){
  queue<string> myqueue;
  string s="bc?aed??";
  string k;



  for(int i=0; i<s.size(); i++){
    k=s.substr(i, 1);
    cout << i << " " << k << endl;


  }



  if(s.find("?") != string::npos && s.size()>1){
    s.erase(remove(s.begin(), s.end(), '?'), s.end());
  }
  cout << s << endl;
  sort(s.begin(), s.end());
  cout << s << endl;
  myqueue.push(s);

  string current=myqueue.front();
  cout << current.size() << current;

  for(int i=0; i<current.size(); i++){
    k=current.substr(i, 1);
    cout << k << " ";}

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
