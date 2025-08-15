
#ifndef CLIPSFACTORY_H
#define CLIPSFACTORY_H

#include "clips_api_6_4.h"
#include "core/variant/variant.h"

  Variant data_object_to_variant(Environment *, UDFValue* );
  Variant data_object_to_variant(Environment *, UDFValue& );

  UDFValue* variant_to_data_object(Environment *, const Variant&, UDFValue *, bool);

  void variant_to_clips_value(Environment *, const Variant &, CLIPSValue &, bool);


#endif // CLIPSFACTORY_H
