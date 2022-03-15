#include <iostream>
#include <fstream>
#include <pqxx/pqxx>

#include "exerciser.h"

using namespace std;
using namespace pqxx;


/**
 * Function in initialization phase
 */

/**
 * drop 4 tables
 */
void dropTables(connection &c) {
  work w(c);
  w.exec("DROP TABLE IF EXISTS PLAYER;");
  w.exec("DROP TABLE IF EXISTS TEAM;");
  w.exec("DROP TABLE IF EXISTS STATE;");
  w.exec("DROP TABLE IF EXISTS COLOR;");    
  w.commit();
  //cout << "Old tables dropped" << endl;
}

/**
 * create 4 tables
 */
void createTables(connection & c) {
  work W(c);
  string sql = "CREATE TABLE STATE("\
    "STATE_ID SERIAL,"              \
    "NAME VARCHAR(256),"      \
    "PRIMARY KEY (STATE_ID)"  \
    ");";
  W.exec(sql);

  sql = "CREATE TABLE COLOR(" \
    "COLOR_ID SERIAL,"        \
    "NAME VARCHAR(256),"      \
    "PRIMARY KEY (COLOR_ID) " \
    ");";
  W.exec(sql);

  sql = "CREATE TABLE TEAM("                                            \
    "TEAM_ID SERIAL,"                                                   \
    "NAME VARCHAR(256), "                                               \
    "STATE_ID INT, "                                                    \
    "COLOR_ID INT,"                                                     \
    "WINS INT,"                                                         \
    "LOSSES INT,"                                                       \
    "PRIMARY KEY (TEAM_ID),"                                            \
    "FOREIGN KEY (STATE_ID) REFERENCES STATE(STATE_ID) ON DELETE SET NULL ON UPDATE CASCADE," \
    "FOREIGN KEY (COLOR_ID) REFERENCES COLOR(COLOR_ID) ON DELETE SET NULL ON UPDATE CASCADE" \
    ");";
  W.exec(sql);
  
  sql = "CREATE TABLE PLAYER("\
    "PLAYER_ID SERIAL,"                                                 \
    "TEAM_ID INT,"                                                      \
    "UNIFORM_NUM INT,"                                                  \
    "FIRST_NAME VARCHAR(256),"                                          \
    "LAST_NAME VARCHAR(256),"                                           \
    "MPG INT,"                                                          \
    "PPG INT,"                                                          \
    "RPG INT,"                                                          \
    "APG INT,"                                                          \
    "SPG DOUBLE PRECISION,"                                             \
    "BPG DOUBLE PRECISION,"                                             \
    "PRIMARY KEY (PLAYER_ID),"                                          \
    "FOREIGN KEY (TEAM_ID) REFERENCES TEAM(TEAM_ID) ON DELETE SET NULL ON UPDATE CASCADE" \
    ");";
  W.exec(sql);
  W.commit();
  //cout << "Table created successfully" << endl;    
}

void initializeColor(connection & c) {
  ifstream file("color.txt");
  string line;  
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      p = line.find(' ');
      pre = p+1;
      add_color(&c, line.substr(pre));
    }
    file.close();
    //cout << "Table color initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file color.txt");
  }
}

void initializeState(connection & c) {
  ifstream file("state.txt");
  string line;  
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      p = line.find(' ');
      pre = p+1;
      add_state(&c, line.substr(pre));
    }
    file.close();
    //    cout << "Table state initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file state.txt");
  }
}

void initializeTeam(connection & c) {
  ifstream file("team.txt");
  string line;
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      p = line.find(' ');
      pre = p+1;
      p = line.find(' ', pre);
      string name = line.substr(pre, p-pre);
      int args[4];
      for (int i =0; i < 4; i++) {
        pre = p+1;
        p = line.find(' ',pre);
        if (i == 3) {
          args[i] = atoi(line.substr(pre).c_str());
        }
        else {          
          args[i] = atoi(line.substr(pre, p-pre).c_str());
        }
      }
      add_team(&c, name, args[0], args[1], args[2], args[3]);
    }
    file.close();
    //cout << "Table team initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file team.txt");
  }
}

void initializePlayer(connection & c) {
  ifstream file("player.txt");
  string line;
  if (file.is_open())   {
    int ints[6];
    double doubles[2];
    string names[2];
    while (getline(file, line)) {
      int p, pre = 0;
      for (int i = 0;i < 3; i++) {        
        p = line.find(' ', pre);
        if (i > 0) {
          ints[i-1] = atoi(line.substr(pre, p-pre).c_str());
        }
        pre = p+1;
      }
      p = line.find(' ', pre);
      names[0] = line.substr(pre, p-pre);
      pre = p+1;
      p = line.find(' ', pre);
      names[1] =  line.substr(pre, p-pre);
      for (int i = 2; i<6; i++) {
        pre = p+1;
        p = line.find(' ',pre);
        ints[i] = atoi(line.substr(pre, p-pre).c_str());
      }
      pre = p+1;
      p = line.find(' ',pre);
      doubles[0] = stod(line.substr(pre, p-pre));
      pre = p+1;
      p = line.find(' ');
      doubles[1] = stod(line.substr(pre, p-pre));
      add_player(&c, ints[0], ints[1], names[0], names[1], ints[2], ints[3], ints[4], ints[5], doubles[0], doubles[1]);
    }
    file.close();
    //cout << "Table player initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file player.txt");
  }
}

void initializeTables(connection & c) {
  initializeColor(c);
  initializeState(c);
  initializeTeam(c);
  initializePlayer(c);
}

/**
 * End of function in initialization phase
 */

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      //cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }

  //clear all existing data on database ACC_BBALL
  dropTables(*C);
  
  // create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database  
  createTables(*C);

  //TODO: load each table with rows from the provided source txt files
  initializeTables(*C);
  
  // exercise select queries
  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


