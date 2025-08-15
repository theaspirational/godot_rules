
#include <string>

#include "factory.h"
#include "clips_api_6_4.h"
#include "core/object/object.h"
#include "core/string/print_string.h"
#include <memory>

  Variant data_object_to_variant(Environment *env, UDFValue* clipsdo) {
    return data_object_to_variant(env,  *clipsdo );
  }

  Variant data_object_to_variant(Environment *env, UDFValue& clipsdo ) {

    Variant value;
    Array values;
    Instance *instancePtr;
    std::string inst_name;
    std::string inst_id_str;
    auto parse_object_id = [](const std::string &text, uint64_t &out_object_id) -> bool {
      if (text.empty()) {
        return false;
      }
      uint64_t value = 0;
      for (char ch : text) {
        if (ch < '0' || ch > '9') {
          return false;
        }
        uint64_t digit = static_cast<uint64_t>(ch - '0');
        if (value > (std::numeric_limits<uint64_t>::max() - digit) / 10ULL) {
          return false; // overflow
        }
        value = value * 10ULL + digit;
      }
      out_object_id = value;
      return true;
    };
    std::string delimiter = ":";

    Multifield* mfptr;

    switch ( clipsdo.header->type )
    {
      case VOID_TYPE:
        return value;
      case STRING_TYPE:
        value = clipsdo.lexemeValue->contents;
        return value;
      case INSTANCE_NAME_TYPE:
        inst_name = clipsdo.lexemeValue->contents;
        {
          size_t colon_pos = inst_name.rfind(delimiter);
          if (colon_pos == std::string::npos || colon_pos + 1 >= inst_name.size()) {
            print_line("clips_es:data_object_to_variant: Invalid instance name (missing colon): " + String(inst_name.c_str()));
            return value;
          }
          inst_id_str = inst_name.substr(colon_pos + 1);
          uint64_t object_id_value = 0;
          if (!parse_object_id(inst_id_str, object_id_value)) {
            print_line("clips_es:data_object_to_variant: Invalid instance id: " + String(inst_id_str.c_str()));
            return value;
          }
          value = ObjectDB::get_instance(ObjectID(object_id_value));
        }
        return value;
      case SYMBOL_TYPE:
        if (clipsdo.lexemeValue == TrueSymbol(env))
        value = true;
        else if (clipsdo.lexemeValue == FalseSymbol(env))
        value = false;
        else
        value = clipsdo.lexemeValue->contents;
        return value;
      case FLOAT_TYPE:
        value = clipsdo.floatValue->contents;
        return value;
      case INTEGER_TYPE:
        value = int64_t(clipsdo.integerValue->contents);
        return value;
      case INSTANCE_ADDRESS_TYPE:
        instancePtr = clipsdo.instanceValue;
        {
          CLIPSValue inst_cv;
          DirectGetSlot(instancePtr,"inst_id",&inst_cv);
          value = ObjectDB::get_instance(ObjectID(static_cast<uint64_t>(inst_cv.integerValue->contents)));
        }
        return value;
      case EXTERNAL_ADDRESS_TYPE:
        print_line("clips_es:data_object_to_variant: Detected EXTERNAL_ADDRESS");
        value = clipsdo.externalAddressValue->contents;
        return value;

      case MULTIFIELD_TYPE:
          mfptr = clipsdo.multifieldValue;
          for ( size_t iter = 0; iter < mfptr->length; iter++ )
            {
              CLIPSValue &cv = mfptr->contents[iter];
              switch ( cv.header->type )
              {
              	case STRING_TYPE:
              	  value = cv.lexemeValue->contents;
              	  values.push_back( Variant( value ) );
              	  break;
              	case SYMBOL_TYPE:
                  if ((iter == 0) && (cv.lexemeValue && strcmp(cv.lexemeValue->contents, "Vector2") == 0)){
                    float x;
                    float y;
                    
                    if (mfptr->contents[iter+1].header->type == FLOAT_TYPE){
                    x = mfptr->contents[iter+1].floatValue->contents;
                    } else if (mfptr->contents[iter+1].header->type == INTEGER_TYPE) {
                    x = (float)int64_t(mfptr->contents[iter+1].integerValue->contents);
                    } else { 
                      print_line("clips_es:data_object_to_variant: Vector2 `x` must be a number"); 
                      return value;
                      }
                    
                    if (mfptr->contents[iter+2].header->type == FLOAT_TYPE) {
                    y = mfptr->contents[iter+2].floatValue->contents;
                    } else if (mfptr->contents[iter+2].header->type == INTEGER_TYPE) {
                    y = (float)int64_t(mfptr->contents[iter+2].integerValue->contents);
                    } else {
                      print_line("clips_es:data_object_to_variant: Vector2 `y` must be a number");
                      return value;
                      }

                    value = Vector2(x,y);
                    return value;
                  } else {
              	  if (cv.header->type == SYMBOL_TYPE || cv.header->type == STRING_TYPE) {
                      value = cv.lexemeValue->contents;
                    } else if (cv.header->type == FLOAT_TYPE) {
                      value = cv.floatValue->contents;
                    } else if (cv.header->type == INTEGER_TYPE) {
                      value = int64_t(cv.integerValue->contents);
                    } else if (cv.header->type == INSTANCE_NAME_TYPE) {
                      value = cv.lexemeValue->contents;
                    } else if (cv.header->type == EXTERNAL_ADDRESS_TYPE) {
                      value = cv.externalAddressValue->contents;
                    } else if (cv.header->type == INSTANCE_ADDRESS_TYPE) {
                      CLIPSValue inst_cv;
                      DirectGetSlot(cv.instanceValue, "inst_id", &inst_cv);
                      value = ObjectDB::get_instance(ObjectID(static_cast<uint64_t>(inst_cv.integerValue->contents)));
                    } else {
                      print_line("clips_es:data_object_to_variant: Unhandled multifield element type");
                      return value;
                    }
              	  values.push_back( Variant( value ) );
              	  break;}
              	case FLOAT_TYPE:
              	  value = cv.floatValue->contents;
              	  values.push_back( Variant( value ) );
              	  break;
              	case INTEGER_TYPE:
              	  value = int64_t(cv.integerValue->contents);
              	  values.push_back( Variant( value ) );
              	  break;
                case INSTANCE_NAME_TYPE:
                  inst_name = cv.lexemeValue->contents;
                  {
                    size_t colon_pos = inst_name.rfind(delimiter);
                    if (colon_pos == std::string::npos || colon_pos + 1 >= inst_name.size()) {
                      print_line("clips_es:data_object_to_variant: Invalid instance name in multifield: " + String(inst_name.c_str()));
                      break;
                    }
                    inst_id_str = inst_name.substr(colon_pos + 1);
                    uint64_t object_id_value = 0;
                    if (!parse_object_id(inst_id_str, object_id_value)) {
                      print_line("clips_es:data_object_to_variant: Invalid instance id in multifield: " + String(inst_id_str.c_str()));
                      break;
                    }
                    value = ObjectDB::get_instance(ObjectID(object_id_value));
                    values.push_back( Variant( value ) );
                  }
                  break;
                case INSTANCE_ADDRESS_TYPE:
                  {
                    CLIPSValue inst_cv;
                    DirectGetSlot(cv.instanceValue,"inst_id",&inst_cv);
                    value = ObjectDB::get_instance(ObjectID(static_cast<uint64_t>(inst_cv.integerValue->contents)));
                  }
                  values.push_back( Variant( value ) );
                  break;
              	case EXTERNAL_ADDRESS_TYPE:
              	  value = cv.externalAddressValue->contents;
              	  values.push_back( Variant( value ) );
              	  break;
              	default:
                  print_line("clips_es:data_object_to_variant: Unhandled multifield type");
            	}
            }
          value = values;
          return value;
      default:
        print_line("clips_es::data_object_to_variant: Unhandled data object type");
    }
    value = false;
    return value;
  }


  UDFValue *
variant_to_data_object(Environment *env, const Variant & value, UDFValue *clipsdo, bool as_symbols)
  {
    Array values;
    Vector2 v2;
    Object *ref;
    String inst_name;

    if (! clipsdo) {
      clipsdo = memnew(UDFValue);
      }

    switch ( value.get_type() ) {
      case Variant::Type::BOOL:
        clipsdo->value = CreateBoolean(env, (bool)value);
        return clipsdo;
      case Variant::Type::NODE_PATH:
      case Variant::Type::STRING:
        if ( as_symbols ) {
          clipsdo->value = CreateSymbol(env, String(value).ascii().get_data());
        } else {
          clipsdo->value = CreateString(env, String(value).ascii().get_data());
        }
        return clipsdo;
      case Variant::Type::INT:
        clipsdo->integerValue = CreateInteger(env, (long long) (int64_t)value);
        return clipsdo;
      case Variant::Type::FLOAT:
        clipsdo->floatValue = CreateFloat(env, (double) (float)value);
        return clipsdo;
      case Variant::Type::OBJECT:
        ref = value;
        inst_name = ref->call("to_string");
        inst_name = inst_name.lstrip("[").rstrip("]");
        clipsdo->value = CreateInstanceName(env, inst_name.ascii().get_data());
        return clipsdo;
      case Variant::Type::NIL:
        clipsdo->value = CreateSymbol(env, "NULL");
        return clipsdo;
      case Variant::Type::VECTOR2:
        v2 = value;
        {
          Multifield *mf = CreateMultifield(env, 3);
          mf->contents[0].lexemeValue = CreateSymbol(env, "Vector2");
          mf->contents[1].floatValue = CreateFloat(env, (double)v2.x);
          mf->contents[2].floatValue = CreateFloat(env, (double)v2.y);
          clipsdo->multifieldValue = mf;
          clipsdo->begin = 0;
          clipsdo->range = mf->length;
        }
        return clipsdo;
      case Variant::Type::PACKED_BYTE_ARRAY:
		  case Variant::Type::PACKED_INT32_ARRAY:
		  case Variant::Type::PACKED_FLOAT32_ARRAY:
		  case Variant::Type::PACKED_STRING_ARRAY:
      case Variant::Type::ARRAY:
        values = value;
        {
          Multifield *mf = CreateMultifield(env, values.size());
          for (unsigned int iter = 0; iter < values.size(); iter++) {
            switch ( values[iter].get_type() ) {
            case Variant::Type::BOOL:
              mf->contents[iter].lexemeValue = CreateBoolean(env, (bool)values[iter]);
              break;
            case Variant::Type::NODE_PATH:
            case Variant::Type::STRING:
              if ( as_symbols ) {
                mf->contents[iter].lexemeValue = CreateSymbol( env,String(values[iter]).ascii().get_data());
              } else {
                mf->contents[iter].lexemeValue = CreateString( env,String(values[iter]).ascii().get_data());
              }
              break;
            case Variant::Type::INT:
              mf->contents[iter].integerValue = CreateInteger( env, (long long) (int64_t)values[iter]);
              break;
            case Variant::Type::FLOAT:
              mf->contents[iter].floatValue = CreateFloat( env, (double) (float)values[iter] );
              break;
            case Variant::Type::NIL:
              mf->contents[iter].lexemeValue = CreateSymbol( env, "NULL");
              break;
            case Variant::Type::OBJECT:
              ref = values[iter];
              inst_name = ref->call("to_string");
              inst_name = inst_name.lstrip("[").rstrip("]");
              mf->contents[iter].value = CreateInstanceName( env, inst_name.ascii().get_data() );
              break;
            case Variant::Type::ARRAY:
              mf->contents[iter].lexemeValue = CreateSymbol( env,"[NESTED_ARRAY]");
              break;
            break;
            default:
              print_line("clips_es::variant_to_data_object ARRAY: Unhandled data object type: "+values[iter].get_type_name(values[iter].get_type()));
          }
          }
          clipsdo->multifieldValue = mf;
          clipsdo->begin = 0;
          clipsdo->range = mf->length;
        }
        return clipsdo;
      default:
        print_line("clips_es::variant_to_data_object: Unhandled data object type: "+value.get_type_name(value.get_type()));
    }

    clipsdo->lexemeValue = FalseSymbol(env);
    return clipsdo;
  }

  void variant_to_clips_value(Environment *env, const Variant & value, CLIPSValue &cv, bool as_symbols) {
    UDFValue temp;
    variant_to_data_object(env, value, &temp, as_symbols);
    if (temp.header && temp.header->type == MULTIFIELD_TYPE) {
      // Full multifield, directly reference underlying storage
      cv.multifieldValue = temp.multifieldValue;
    } else {
      cv.value = temp.value;
    }
  }
