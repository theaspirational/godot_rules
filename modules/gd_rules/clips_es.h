#ifndef CLIPS_ES_H
#define CLIPS_ES_H


#include "clips_api_6_4.h"
#include "Router.h"
#include "scene/main/node.h"


extern "C" void godot_step_function(Environment *, UDFContext *, UDFValue *);
extern "C" void godot_steps_function(Environment *, UDFContext *, UDFValue *);
extern "C" void godot_dofast_function(Environment *, UDFContext *, UDFValue *);
extern "C" void godot_do_function(Environment *, UDFContext *, UDFValue *);
extern "C" void godot_dopipe_function(Environment *, UDFContext *, UDFValue *);


class CLIPS_ENV : public RefCounted {
    GDCLASS(CLIPS_ENV, RefCounted);

protected:
    static void _bind_methods();

private:
    Environment *env;
    ROUTER r;

public:
    CLIPS_ENV();
    //~CLIPS_ENV();

    Variant get_class_slots( String, int );
    Variant get_defclasses();
    void set_ingame_node( Variant );
    void make_instance( String );
    int unmake_instance( String );
    int delete_instance( String );
    int set_slot( String, String, Variant, bool );
    void assert_string( String );
    int build( String );
    int load( String );
    int run( int );
    void reset();
    void clear();
    Variant eval( String );
};

#endif // CLIPS_ES_H
