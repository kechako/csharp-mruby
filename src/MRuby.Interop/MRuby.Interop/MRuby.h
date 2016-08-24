// MRuby.h

#pragma once

#include <msclr\marshal.h>
#include <mruby.h>
#include <mruby\compile.h>

#include "MRubyClass.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

namespace MRubyCSharp {
	namespace Interop {

		ref class MRubyClass;

		/// <summary>
		/// </summary>
		public ref class MRuby
		{
		private:
			mrb_state* state;
			RClass *csharpClass;
			SortedList<String^, MRubyClass^>^ classList;

		public:
			MRuby();
			~MRuby();
			Object^ LoadString(String^ s);
			MRubyClass ^CreateClass(Type ^type, String ^name);

		private:
			void InitCSharpModule();
			mrb_value CSharpLoad(mrb_state* state, mrb_value self);
		};
	}
}
