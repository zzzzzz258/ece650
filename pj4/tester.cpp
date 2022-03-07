#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main() {  
  ifstream file("player.txt");
  string line;
  string presql("INSERT INTO PLAYER (PLAYER_ID, TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES (");
  string sufsql(");\n");
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      ss << presql;
      for (int i = 0;i < 3; i++) {        
        p = line.find(' ', pre);
        if (i > 0) {
          ss << ", ";
        }
        ss << line.substr(pre, p-pre);
        pre = p+1;
      }
      p = line.find(' ', pre);
      ss << ", '" << line.substr(pre, p-pre) << "'";
      pre = p+1;
      p = line.find(' ', pre);
      ss << ", '" << line.substr(pre, p-pre) << "'";
      while (true) {
        pre = p+1;
        p = line.find(' ',pre);
        if (p == string::npos) {
          ss << ", " << line.substr(pre);
          break;
        }
        else {          
          ss << ", " << line.substr(pre, p-pre);
        }
      }      
      ss << sufsql;
    }
    file.close();
    cout << ss.str() << endl;
    cout << "Table player initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file player.txt");
  }
}
