#pragma once

#include <msclr\marshal.h>
#include <mruby.h>
#include <mruby\string.h>

#ifndef mrb_bool_p
#define mrb_bool_p(o)	((mrb_type(o) == MRB_TT_FALSE && mrb_fixnum(o)) || mrb_type(o) == MRB_TT_TRUE)
#endif

using namespace System;
using namespace msclr::interop;

namespace MRubyCSharp {
	namespace Interop {
		static ref class MRubyUtils
		{
		public:
			static Object^ GetCSharpValue(mrb_state* state, mrb_value value);
			static mrb_value GetMRubyValue(mrb_state* state, Object^ obj);
		};
	}
}
