#include <iostream>
#include <pqxx/pqxx>

#include "exerciser.h"

using namespace std;
using namespace pqxx;


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

  
  // exercise select queries
  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


