// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "MRuby.Interop.h"

mrb_value get_mrb_value_from_cli_obj(mrb_state* mrb, Object^ obj) {
	if (obj == nullptr) {
		return mrb_nil_value();
	}

	Type^ type = obj->GetType();
	if (type == SByte::typeid ||
		type == Int16::typeid ||
		type == Int32::typeid ||
		type == Int64::typeid ||
		type == Byte::typeid ||
		type == UInt16::typeid ||
		type == UInt32::typeid ||
		type == UInt64::typeid) {
		return mrb_fixnum_value((mrb_int)obj);
	}
	else if (type == Single::typeid ||
		type == Double::typeid) {
		return mrb_float_value(mrb, (mrb_float)obj);
	}
	else if (type == Boolean::typeid) {
		if ((Boolean^)obj) {
			return mrb_true_value();
		}
		else {
			return mrb_false_value();
		}
	}
	else if (type == String::typeid) {
		marshal_context ctx;
		return mrb_str_new_cstr(mrb, ctx.marshal_as<const char*>((String^)obj));
	}
	else if (type == MRuby::Interop::MRubyObject::typeid) {
		return *(((MRuby::Interop::MRubyObject^)obj)->value);
	}
	else {
		throw gcnew ArgumentException("サポートしていない型がパラメーターに指定されました。");
	}
}

Object^ get_cli_obj_from_mrb_value(MRuby::Interop::MRubyInterpreter^ mri, mrb_value value) {
	if (mrb_nil_p(value)) {
		return nullptr;
	}
	else if (mrb_bool_p(value)) {
		return (Boolean^)mrb_bool(value);
	}
	else if (mrb_fixnum_p(value)) {
		return (Int32^)mrb_fixnum(value);
	}
	else if (mrb_float_p(value)) {
		return (Double^)mrb_float(value);
	}
	else if (mrb_string_p(value)) {
		char* str = mrb_str_to_cstr(mri->mrb_state, value);

		marshal_context ctx;
		return ctx.marshal_as<String^>(str);
	}
	else {
		return gcnew MRuby::Interop::MRubyObject(mri, value);
	}
}

MRuby::Interop::MRubyInterpreter::MRubyInterpreter() {
	this->mrb_state = mrb_open();
}
MRuby::Interop::MRubyInterpreter::~MRubyInterpreter() {
	mrb_close(this->mrb_state);
}

void MRuby::Interop::MRubyInterpreter::LoadString(String^ code) {
	marshal_context ctx;
	mrb_load_string(this->mrb_state, ctx.marshal_as<const char*>(code));
}
MRuby::Interop::MRubyClass^ MRuby::Interop::MRubyInterpreter::GetClass(String^ name) {
	marshal_context ctx;

	RClass* c = mrb_class_get(this->mrb_state, ctx.marshal_as<const char*>(name));
	return gcnew MRubyClass(this, c);
}
void MRuby::Interop::MRubyInterpreter::P(Object^ obj) {
	mrb_value value = get_mrb_value_from_cli_obj(this->mrb_state, obj);
	mrb_p(this->mrb_state, value);
}
void MRuby::Interop::MRubyInterpreter::FullGC() {
	mrb_full_gc(this->mrb_state);
}

MRuby::Interop::MRubyClass::MRubyClass(MRubyInterpreter^ mri, RClass* c) {
	this->mri = mri;
	this->r_class = c;
}

MRuby::Interop::MRubyObject^ MRuby::Interop::MRubyClass::New(... cli::array<Object^>^ args) {
	int len = args->Length;
	mrb_value* argv = (mrb_value*)mrb_malloc(this->mri->mrb_state, sizeof(mrb_value)*len);
	for (int i = 0; i < len; i++) {
		argv[i] = get_mrb_value_from_cli_obj(this->mri->mrb_state, args[i]);
	}

	mrb_value value = mrb_obj_new(this->mri->mrb_state, this->r_class, len, argv);

	mrb_free(this->mri->mrb_state, argv);

	return gcnew MRubyObject(this->mri, value);
}

MRuby::Interop::MRubyObject::MRubyObject(MRubyInterpreter^ mri, mrb_value value) {
	this->mri = mri;
	this->value = &value;
}

MRuby::Interop::MRubyObject::~MRubyObject() {
}

Object^ MRuby::Interop::MRubyObject::Send(String^ name, ... cli::array<Object^>^ args) {
	int len = args->Length;
	mrb_value* argv = (mrb_value*)mrb_malloc(this->mri->mrb_state, sizeof(mrb_value)*len);
	for (int i = 0; i < len; i++) {
		argv[i] = get_mrb_value_from_cli_obj(this->mri->mrb_state, args[i]);
	}
	marshal_context ctx;
	mrb_value value = mrb_funcall_argv(this->mri->mrb_state, *(this->value), mrb_intern_cstr(this->mri->mrb_state, ctx.marshal_as<const char*>(name)), len, argv);

	mrb_free(this->mri->mrb_state, argv);

	return get_cli_obj_from_mrb_value(this->mri, value);
}
