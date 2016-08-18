// MRuby.h

#pragma once

#include <msclr\marshal.h>
#include <mruby.h>
#include <mruby\compile.h>

using namespace System;
using namespace msclr::interop;

namespace MRubyCSharp {
	namespace Interop{
		/// <summary>
		/// </summary>
		public ref class MRuby
		{
		public:
			MRuby();
			~MRuby();
			Object^ LoadString(String^ s);

		private:
			mrb_state* mrb_state;
		};
	}
}
