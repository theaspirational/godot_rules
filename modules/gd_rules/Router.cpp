
#include "Router.h"
#include "clips_api_6_4.h"
#include <cstring>

#include "core/string/print_string.h"
#include "core/string/ustring.h"


String ROUTER::message  = "";


bool ROUTER::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}


int ROUTER::add_to_environment(){
  return AddRouter(env, name, priority,
     query_function,
     print_function,
     NULL,
     NULL,
     NULL,
     NULL) ? 1 : 0;}


bool ROUTER::query_function(Environment * env, const char * name, void *context){
  return true;}


void ROUTER::print_function(Environment * env,  const char *name, const char *m, void *context){
  if (strcmp(m, "\n") == 0)
      {
      print_line(message);
      message = "";}
  else
  { message += m;
    if (hasEnding(m, "\n"))
        {print_line(message);} };
  return;}

