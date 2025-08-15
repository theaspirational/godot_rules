#ifndef CLIPS_ROUTER_H
#define CLIPS_ROUTER_H

#include <string>
#include "core/string/print_string.h"
#include "core/string/ustring.h"
#include "clips_api_6_4.h"

// ROUTER class serves for printing clips messages to external shells (e.g Godot debug)
class ROUTER {

public:
  static String message;
  Environment *env;
  const char *name = "godot_console";
  int  priority = 20;

  static bool hasEnding (std::string const &fullString, std::string const &ending);

  int add_to_environment();
  static bool query_function(Environment *, const char *, void *);
  static void print_function(Environment *, const char *, const char *, void *);

};

#endif // CLIPS_ROUTER_H
