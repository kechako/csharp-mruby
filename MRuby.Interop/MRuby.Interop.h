// MRuby.Interop.h

#include <msclr\marshal_cppstd.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/gc.h>
#include <mruby/string.h>

#ifndef mrb_bool_p
#define mrb_bool_p(o)   ((mrb_type(o) == MRB_TT_FALSE && !mrb_nil_p(o)) || mrb_type(o) == MRB_TT_TRUE)
#endif

#pragma once
using namespace msclr::interop;
using namespace System;

namespace MRuby {
	namespace Interop {
		ref class MRubyInterpreter;
		ref class MRubyClass;
		ref class MRubyObject;

		public ref class MRubyObject
		{
		private:
			MRubyInterpreter^ mri;

		internal:
			mrb_value* value;

			MRubyObject(MRubyInterpreter^ mri, mrb_value value);

		public:
			Object^ Send(String^ name, ... cli::array<Object^>^ args);
			~MRubyObject();
		};

		public ref class MRubyClass {
		private:
			MRubyInterpreter^ mri;
			RClass* r_class;

		internal:
			MRubyClass(MRubyInterpreter^ mri, RClass* c);

		public:
			MRubyObject^ New(... cli::array<Object^>^ args);
		};

		public ref class MRubyInterpreter
		{
		internal:
			mrb_state* mrb_state;
		public:
			MRubyInterpreter();
			~MRubyInterpreter();

			void LoadString(String^ code);
			MRubyClass^ GetClass(String^ name);
			void P(Object^ obj);
			void FullGC();
		};
	}
}
