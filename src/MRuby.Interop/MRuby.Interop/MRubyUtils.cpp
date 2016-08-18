#include "stdafx.h"
#include "MRubyUtils.h"


Object ^ MRubyCSharp::Interop::MRubyUtils::GetCSharpValue(mrb_state* state, mrb_value value)
{
	// nil
	if (mrb_nil_p(value)) {
		return nullptr;
	}
	// Boolean
	else if (mrb_bool_p(value)) {
		return mrb_bool(value);
	}
	// Fixnum
	else if (mrb_fixnum_p(value)) {
		return mrb_fixnum(value);
	}
	// Float
	else if (mrb_float_p(value)) {
		return mrb_float(value);
	}
	// String
	else if (mrb_string_p(value)) {
		marshal_context ctx;
		return ctx.marshal_as<String^>(mrb_str_to_cstr(state, value));
	}
	// Array
	// Hash

	return nullptr;
}

mrb_value MRubyCSharp::Interop::MRubyUtils::GetMRubyValue(mrb_state * state, Object ^ obj)
{
	// nil
	if (obj == NULL) {
		return mrb_nil_value();
	}

	Type^ type = obj->GetType();
	// Boolean
	if (type == Boolean::typeid) {
		if ((Boolean^)obj) {
			return mrb_true_value();
		}
		else {
			return mrb_false_value();
		}
	}
	// Fixnum
	else if (type == Byte::typeid ||
		type == Int16::typeid ||
		type == Int32::typeid ||
		type == Int64::typeid) {
		return mrb_fixnum_value(Convert::ToInt32(obj));
	}
	// Float
	else if (type == Single::typeid ||
		type == Double::typeid) {
		return mrb_float_value(state, (Double^)obj);
	}
	// String
	else if (type == String::typeid) {
		marshal_context ctx;
		return mrb_str_new_cstr(state, ctx.marshal_as<const char*>((String^)obj));
	}

	return mrb_nil_value();
}
