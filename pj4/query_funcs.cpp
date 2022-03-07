#include "query_funcs.h"
#include <iomanip>

void exec_commit(work & w, string sql) {
  w.exec(sql);
  w.commit();
}

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
  work w(*C);
  stringstream sql;
  sql << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, "
         "RPG, APG, SPG, BPG) VALUES ("
      << team_id << ", " << jersey_num << ", " << w.quote(first_name) << ", "
      << w.quote(last_name) << ", " << mpg << ", " << ppg << ", " << rpg << ", " << apg
      << ", " << spg << ", " << bpg << ");";
  exec_commit(w, sql.str());
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
  work w(*C);
  stringstream sql;
  sql << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
      << w.quote(name) << ", " << state_id << ", " << color_id << ", " << wins << ", "
      << losses << ");";
  exec_commit(w, sql.str());
}


void add_state(connection *C, string name)
{
  work w(*C);
  string sql = "INSERT INTO STATE (NAME) VALUES (" + w.quote(name) + ");";
  exec_commit(w, sql);
}


void add_color(connection *C, string name)
{
  work w(*C);
  string sql = "INSERT INTO COLOR (NAME) VALUES (" + w.quote(name) + ");";
  exec_commit(w, sql);
}


void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
  string sql = "select * from player";
  int flags[6] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
  int min_ints[4] = {min_mpg, min_ppg, min_rpg, min_apg};
  int max_ints[4] = {max_mpg, max_ppg, max_rpg, max_apg};
  double min_doubles[2] = {min_spg, min_bpg};
  double max_doubles[2] = {max_spg, max_bpg};
  string col_names[6] = {"MPG", "PPG", "RPG", "APG", "SPG", "BPG"};
  bool first = true;
  
  for (int i = 0 ; i < 6; i++ ) {
    if (flags[i] != 0) {
      if (first) {
        first = false;
        sql += " where ";
      }
      else {
        sql += " and ";
      }
      sql += col_names[i];
      sql += " between ";
      if (i < 4) {
        sql += to_string(min_ints[i]);
        sql += " and ";
        sql += to_string(max_ints[i]);
      } else {
        sql += to_string(min_doubles[i]);
        sql += " and ";
        sql += to_string(max_doubles[i]);
      }
    }
  }
  sql += ";";
  //  cout << sql << endl;
  nontransaction N(*C);
  result R(N.exec(sql));
  cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"
       << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " "
         << c[3].as<string>() << " " << c[4].as<string>() << " " << c[5].as<int>() << " "
         << c[6].as<int>() << " " << c[7].as<int>() << " " << c[8].as<int>() << " "
         << fixed << setprecision(1) << c[9].as<double>() << " " << c[10].as<double>()
         << endl;
  }
}

// show the name of each team with the indicated uniform color
void query2(connection *C, string team_color)
{
  string sql = "select team.name from team inner join color on team.color_id=color.color_id where color.name = '" + C->esc(team_color.c_str()) + "';";
  nontransaction n(*C);
  result r(n.exec(sql));
  cout << "NAME" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << endl;
  }
}

// show the first and last name of each player that plays for the indicated team,
// ordered from highest to lowest ppg (points per game)
void query3(connection *C, string team_name)
{
  string sql = "select first_name, last_name from player inner join team on player.team_id=team.team_id where team.name='" + C->esc(team_name) + "' order by PPG desc";
  nontransaction n(*C);
  result r(n.exec(sql));
  cout << "FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator c = r.begin(); c != r.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
  }
}

/**
 * show first name, last name, and jersey number of each player that plays in the
 * indicated state and wears the indicated uniform color
 */
void query4(connection *C, string team_state, string team_color)
{
  string sql = "select first_name, last_name, uniform_num from player inner join team on player.team_id=team.team_id inner join state on team.state_id=state.state_id inner join color on color.color_id=team.color_id where state.name = '" +C->esc(team_state)  + "' and color.name = '" + C->esc(team_color) + "'";
  nontransaction N(*C);
  result R(N.exec(sql));
  cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " " << c[2].as<int>()
         << endl;
  }
}

/**
 * show first name and last name of each player, and team name and number of
 * wins for each team that has won more than the indicated number of games
 */
void query5(connection *C, int num_wins)
{
  string sql = "select first_name,last_name, team.name, wins from player join team on player.team_id = team.team_id where wins > " + to_string(num_wins) + ";";
  nontransaction N(*C);
  result R(N.exec(sql));
  cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    cout << c[0].as<string>() << " " << c[1].as<string>() << " " << c[2].as<string>()
         << " " << c[3].as<int>() << endl;
  }
}
