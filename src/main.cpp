#include "handler.h"

HANDLER_CONTROL *handler;

void setup()
{
  handler = new HANDLER_CONTROL();
}

void loop()
{
  handler->execute();
}