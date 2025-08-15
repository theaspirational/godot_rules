
#include "clips_es.h"
#include "factory.h"

Node *ingame_singletone;

//*****************************
// external CLIPS functions 
// for <task-pattern> - look in the singletone/CLIPS.gd file at _match_task(t) function
//*****************************


//(. <task-pattern> <argumets>) - adds task to godot flow array - executes on idle
extern "C" void godot_step_function(Environment *env, UDFContext *ctx, UDFValue *ret) {
    
    Variant value;
    String gd_funcname("add_step");
    Array args_arr;
    unsigned int argnum = UDFArgumentCount(ctx);

    for (unsigned int i = 1; i <= argnum; i++)
    {
      UDFValue arg; UDFNthArgument(ctx, i, ANY_TYPE_BITS, &arg);
      args_arr.push_back(data_object_to_variant(env, arg));
    };

    value = ingame_singletone->call(gd_funcname, args_arr);
    variant_to_data_object(env, value, ret, false);
  }


// (.. 
//	-> <task-pattern> <argumets>
//	-> <task-pattern> <argumets>
//	-> ..	) - flow pipeline godot function - adds tasks(steps) to godot flow array
// - executes on iddle frame "step by step", returns TRUE
extern "C" void godot_steps_function(Environment *env, UDFContext *ctx, UDFValue *ret) {

    Variant value;
    String gd_funcname1("add_step");
    String gd_funcname2("add_steps");
    Array final_arr;
    Array temp_arr;
    unsigned int argnum = UDFArgumentCount(ctx);

    UDFValue first; UDFFirstArgument(ctx, LEXEME_BITS, &first);
    if (!(first.lexemeValue && strcmp(first.lexemeValue->contents, "->") == 0))
        {
          print_line("clips_es:godot_steps_function: First argument must be separator '->'");
          ret->lexemeValue = FalseSymbol(env);
          return;
          }

    for (unsigned int i = 2; i <= argnum; i++)
    {
      bool is_separator;
      UDFValue arg; UDFNthArgument(ctx, i, ANY_TYPE_BITS, &arg);
      is_separator = arg.header->type == SYMBOL_TYPE && strcmp(arg.lexemeValue->contents, "->") == 0;

      if ( is_separator ){
        final_arr.append(ingame_singletone->call(gd_funcname1, temp_arr.duplicate()));
        temp_arr.clear();
      } else {
        temp_arr.push_back(data_object_to_variant(env, arg));}
    }

    final_arr.append(ingame_singletone->call(gd_funcname1, temp_arr.duplicate()));
    value = ingame_singletone->call(gd_funcname2, final_arr);

    variant_to_data_object(env, value, ret, false);
  }


// (now ?<gd-obj> <gd-obj_func_name> <arg1> <arg2> <argN> ...) - instant godot function -
// - executes and returns value immediately 
extern "C" void godot_dofast_function(Environment *env, UDFContext *ctx, UDFValue *ret) {

    String gd_funcname;
    Array gd_arguments;
    unsigned int argnum = UDFArgumentCount(ctx);
    Variant value;
    Object *inst;

    UDFValue first; UDFFirstArgument(ctx, ANY_TYPE_BITS, &first);

    if (
        first.header->type == INSTANCE_NAME_TYPE
        ||
        first.header->type == INSTANCE_ADDRESS_TYPE
        )
        inst = data_object_to_variant(env, first);
    else
        {
          print_line("clips_es:godot_dofast_function: First argument in 'now' func must be of type INSTANCE_NAME");
          ret->lexemeValue = FalseSymbol(env);
          return;
        }

    UDFValue second; if (! UDFNthArgument(ctx, 2, SYMBOL_BIT, &second))
        {
          print_line("clips_es:godot_retval_function: Second argument in 'now' func (gd_funcname) must be of type SYMBOL");
          ret->lexemeValue = FalseSymbol(env);
          return;
        }

    gd_funcname = second.lexemeValue->contents;

    for (unsigned int i = 3; i <= argnum; i++)
    {
      UDFValue arg; UDFNthArgument(ctx, i, ANY_TYPE_BITS, &arg);
      gd_arguments.push_back(data_object_to_variant(env, arg));
    }
    // print_line("==> Doing now from clips:");
    // print_line(inst->to_string()+" "+gd_funcname+" "+ingame_singletone->call("arr_to_str", gd_arguments));
    value = inst->callv(gd_funcname, gd_arguments);

    variant_to_data_object(env, value, ret, false);
  }


// (.now <task-pattern> <argumets>) - instant  godot function 
// - executes and returns value immediately 
extern "C" void godot_do_function(Environment *env, UDFContext *ctx, UDFValue *ret) {

    Variant value;
    String gd_funcname("do");
    Array args_arr;
    unsigned int argnum = UDFArgumentCount(ctx);

    for (unsigned int i = 1; i <= argnum; i++)
    {
      UDFValue arg; UDFNthArgument(ctx, i, ANY_TYPE_BITS, &arg);
      args_arr.push_back(data_object_to_variant(env, arg));
    };

    value = ingame_singletone->call(gd_funcname, args_arr);
    variant_to_data_object(env, value, ret, false);
  }


//(..now 
//	-> <task-pattern> <argumets>
//	-> <task-pattern> <argumets>
//	-> ..	) - instant pipeline godot function - executes and returns !last value immediately 
extern "C" void godot_dopipe_function(Environment *env, UDFContext *ctx, UDFValue *ret) {

    Variant value;
    String gd_funcname("dopipe");
    Array final_arr;
    Array temp_arr;
    unsigned int argnum = UDFArgumentCount(ctx);

    UDFValue first; UDFFirstArgument(ctx, LEXEME_BITS, &first);
    if (!(first.lexemeValue && strcmp(first.lexemeValue->contents, "->") == 0))
        {
          print_line("clips_es:godot_dopipe_function: First argument in '..now' func must be separator '->'");
          ret->lexemeValue = FalseSymbol(env);
          return;
          }

    for (unsigned int i = 2; i <= argnum; i++)
    {
      bool is_separator;
      UDFValue arg; UDFNthArgument(ctx, i, ANY_TYPE_BITS, &arg);
      is_separator = arg.header->type == SYMBOL_TYPE && strcmp(arg.lexemeValue->contents, "->") == 0;

      if ( is_separator ){
        final_arr.append(temp_arr.duplicate());
        temp_arr.clear();
      } else {
        temp_arr.push_back(data_object_to_variant(env, arg));}
    }

    final_arr.append(temp_arr.duplicate());
    value = ingame_singletone->call(gd_funcname, final_arr);

    variant_to_data_object(env, value, ret, false);
  }



//*****************************
// CLIPS_ENV:: functions 
//*****************************
CLIPS_ENV::CLIPS_ENV() {
    env = CreateEnvironment();
    r =  ROUTER();
    r.env = env;
    r.add_to_environment();

    // Register UDFs for Godot interop in CLIPS 6.4
    AddUDF(env, "now", "*", 2, UNBOUNDED, "*", godot_dofast_function, "godot_dofast_function", NULL);
    AddUDF(env, ".now", "*", 1, UNBOUNDED, "*", godot_do_function, "godot_do_function", NULL);
    AddUDF(env, "..now", "*", 2, UNBOUNDED, "*", godot_dopipe_function, "godot_dopipe_function", NULL);
    AddUDF(env, ".", "*", 1, UNBOUNDED, "*", godot_step_function, "godot_step_function", NULL);
    AddUDF(env, "..", "*", 1, UNBOUNDED, "*", godot_steps_function, "godot_steps_function", NULL);
};


Variant CLIPS_ENV::get_class_slots(String defclassName, int inheritFlag) {
  UDFValue temp;
  {
    CLIPSValue cv; ClassSlots(FindDefclass(env,defclassName.ascii().get_data()), &cv, inheritFlag != 0);
    CLIPSToUDFValue(&cv, &temp);
  }
  return data_object_to_variant(env, temp);
}


Variant CLIPS_ENV::get_defclasses() {
  UDFValue temp;
  {
    CLIPSValue cv; GetDefclassList(env, &cv, NULL);
    CLIPSToUDFValue(&cv, &temp);
  }
  return data_object_to_variant(env, temp);
}


void CLIPS_ENV::set_ingame_node( Variant node){
  ingame_singletone = Object::cast_to<Node>(node);
  return;};


void CLIPS_ENV::make_instance(String makeCommand) {
  MakeInstance(env, makeCommand.ascii().get_data());}


// This function is equivalent to DeleteInstance except 
// that it uses message-passing instead of directly deleting the instance(s).
int CLIPS_ENV::unmake_instance(String instanceName) {
  instanceName = instanceName.lstrip("[").rstrip("]");
  Instance *instancePtr = FindInstance(env,NULL,instanceName.ascii().get_data(),false);
  return (int) UnmakeInstance(instancePtr);
}


int CLIPS_ENV::delete_instance(String instanceName) {
  instanceName = instanceName.lstrip("[").rstrip("]");
  Instance *instancePtr = FindInstance(env,NULL,instanceName.ascii().get_data(),false);
  return (int) DeleteInstance(instancePtr);
}


int CLIPS_ENV::set_slot(String instanceName, String slotName, Variant inputValue, bool as_symbols) {
  instanceName = instanceName.lstrip("[").rstrip("]");
  Instance *instancePtr = FindInstance(env, NULL, instanceName.ascii().get_data(), false);
  CLIPSValue cv; variant_to_clips_value(env, inputValue, cv, as_symbols);
  PutSlotError pe = DirectPutSlot(instancePtr, slotName.ascii().get_data(), &cv);
  return (pe == PSE_NO_ERROR) ? 1 : 0;
}


void CLIPS_ENV::assert_string(String string) {
 	AssertString(env, string.ascii().get_data());
}


int CLIPS_ENV::build(String constructString) {
	return Build(env, constructString.ascii().get_data()) == BE_NO_ERROR ? 1 : 0;
}


int CLIPS_ENV::load(String filename) {
	{
	  LoadError le = Load(env, filename.ascii().get_data());
	  if (le == LE_NO_ERROR) return 1;        // opened and no errors occurred
	  if (le == LE_OPEN_FILE_ERROR) return 0; // couldn't open file
	  return -1;                               // opened but error occurred while loading
	}
}


int CLIPS_ENV::run( int runlimit ) {
  return (int) Run( env, runlimit );
}


void CLIPS_ENV::reset() {
	Reset(env);
}


void CLIPS_ENV::clear() {
	Clear(env);
}


Variant CLIPS_ENV::eval(String expressionString) {
  UDFValue temp;
  {
    CLIPSValue cv; Eval(env, expressionString.ascii().get_data(), &cv);
    CLIPSToUDFValue(&cv, &temp);
  }
  return data_object_to_variant(env, temp);
}


void CLIPS_ENV::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_ingame_node"),
  &CLIPS_ENV::set_ingame_node);
  ClassDB::bind_method(D_METHOD("build"),
  &CLIPS_ENV::build);
  ClassDB::bind_method(D_METHOD("set_slot"),
  &CLIPS_ENV::set_slot);
  ClassDB::bind_method(D_METHOD("get_class_slots"),
  &CLIPS_ENV::get_class_slots);
  ClassDB::bind_method(D_METHOD("get_defclasses"),
  &CLIPS_ENV::get_defclasses);
  ClassDB::bind_method(D_METHOD("assert_string"),
  &CLIPS_ENV::assert_string);
  ClassDB::bind_method(D_METHOD("make_instance"),
  &CLIPS_ENV::make_instance);
  ClassDB::bind_method(D_METHOD("unmake_instance"),
  &CLIPS_ENV::unmake_instance);
  ClassDB::bind_method(D_METHOD("delete_instance"),
  &CLIPS_ENV::delete_instance);
  ClassDB::bind_method(D_METHOD("eval"),
  &CLIPS_ENV::eval);
  ClassDB::bind_method(D_METHOD("load"),
  &CLIPS_ENV::load);
  ClassDB::bind_method(D_METHOD("clear"),
  &CLIPS_ENV::clear);
  ClassDB::bind_method(D_METHOD("reset"),
  &CLIPS_ENV::reset);
  ClassDB::bind_method(D_METHOD("run"),
  &CLIPS_ENV::run);};