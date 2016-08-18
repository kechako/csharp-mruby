// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "MRuby.h"
#include "MRubyUtils.h"

MRubyCSharp::Interop::MRuby::MRuby() {
	this->mrb_state = mrb_open();
}
MRubyCSharp::Interop::MRuby::~MRuby() {
	mrb_close(this->mrb_state);
}

Object^ MRubyCSharp::Interop::MRuby::LoadString(String ^ s)
{
	marshal_context ctx;
	mrb_value value = mrb_load_string(this->mrb_state, ctx.marshal_as<const char*>(s));

	return MRubyUtils::GetCSharpValue(this->mrb_state, value);
}
