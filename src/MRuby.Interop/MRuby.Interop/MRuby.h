// MRuby.h

#pragma once

#include <msclr\marshal.h>
#include <mruby.h>
#include <mruby\compile.h>

#include "MRubyClass.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

namespace MRubyCSharp {
	namespace Interop {

		ref class MRubyClass;


		/// <summary>
		/// </summary>
		public ref class MRuby
		{
		public:
			delegate mrb_value MRubyFunc(mrb_state *mrb, mrb_value self);


		private:
			String ^moduleName;
			mrb_state *mrb;
			RClass *csharpClass;
			List<Assembly^>^ assemblyList;
			SortedList<String^, MRubyClass^>^ classList;
			MRubyFunc^ funcTest;
			MRubyFunc^ funcLoad;
			MRubyFunc^ funcUsing;

		public:
			MRuby(String ^moduleName);
			~MRuby();
			Object^ LoadString(String^ s);
			MRubyClass ^CreateClass(Type ^type, String ^name);

		private:
			void InitCSharpModule();
			mrb_value CSharpTest(mrb_state *mrb, mrb_value self);
			mrb_value CSharpLoad(mrb_state *mrb, mrb_value self);
			mrb_value CSharpUsing(mrb_state *mrb, mrb_value self);
		};
	}
}
