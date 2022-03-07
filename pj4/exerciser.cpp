#include "exerciser.h"

void exercise(connection *C)
{
  query2(C, "Maroon");
  query3(C, "Duke");
  query4(C, "NC", "DarkBlue");
  query5(C, 13);
  query1(C, 0,0,0, 1, 10,20, 0,0,0, -1, 5,6, 0,0,0, 0,0,0);
  query1(C, 2,35,40, 1, 10,20, 0,0,0, 0, 5,6, 0,0,0, 0,0,0);
}
