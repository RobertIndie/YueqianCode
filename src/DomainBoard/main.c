#include "../../lib/db.h"

int main()
{
   struct Controller* controller = ConfigLoad("config.txt");
   return Run(controller);
}