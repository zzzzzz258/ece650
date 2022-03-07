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
  cout << "Old tables dropped" << endl;
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
    "MPG DOUBLE PRECISION,"                                                          \
    "PPG DOUBLE PRECISION,"                                                          \
    "RPG DOUBLE PRECISION,"                                                          \
    "APG DOUBLE PRECISION,"                                                          \
    "SPG DOUBLE PRECISION,"                                             \
    "BPG DOUBLE PRECISION,"                                             \
    "PRIMARY KEY (PLAYER_ID),"                                          \
    "FOREIGN KEY (TEAM_ID) REFERENCES TEAM(TEAM_ID) ON DELETE SET NULL ON UPDATE CASCADE" \
    ");";
  W.exec(sql);
  W.commit();
  cout << "Table created successfully" << endl;    
}

void initializeColor(connection & c) {
  work w(c);
  ifstream file("color.txt");
  string line;
  string presql("INSERT INTO COLOR (COLOR_ID, NAME) VALUES (");
  string sufsql(");\n");
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      ss << presql;
      p = line.find(' ');
      ss << line.substr(pre, p-pre);
      pre = p+1;
      ss << ", '" << line.substr(pre) << "'";
      ss << sufsql;
    }
    file.close();
    w.exec(ss.str());
    w.commit();
    cout << "Table color initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file color.txt");
  }
}

void initializeState(connection & c) {
  work w(c);
  ifstream file("state.txt");
  string line;
  string presql("INSERT INTO STATE (STATE_ID, NAME) VALUES (");
  string sufsql(");\n");
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      ss << presql;
      p = line.find(' ');
      ss << line.substr(pre, p-pre);
      pre = p+1;
      ss << ", '" << line.substr(pre) << "'";
      ss << sufsql;
    }
    file.close();
    w.exec(ss.str());
    w.commit();
    cout << "Table state initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file state.txt");
  }
}

void initializeTeam(connection & c) {
  work w(c);
  ifstream file("team.txt");
  string line;
  string presql("INSERT INTO TEAM (TEAM_ID, NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES (");
  string sufsql(");\n");
  if (file.is_open())   {
    stringstream ss;
    while (getline(file, line)) {
      int p, pre = 0;
      ss << presql;
      p = line.find(' ');
      ss << line.substr(pre, p-pre);
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
    w.exec(ss.str());
    w.commit();
    cout << "Table team initialized" << endl;
  }
  else {    
    throw new invalid_argument("Cannot open file team.txt");
  }
}

void initializePlayer(connection & c) {
  work w(c);
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
      string first_name = line.substr(pre, p-pre);
      if (first_name.find('\'') != string::npos) {
        int quote = first_name.find('\'');
        first_name.replace(quote, 1, "''");
      }
      ss << ", '" << first_name << "'";
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
    w.exec(ss.str());
    w.commit();
    cout << "Table player initialized" << endl;
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
      cout << "Opened database successfully: " << C->dbname() << endl;
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


