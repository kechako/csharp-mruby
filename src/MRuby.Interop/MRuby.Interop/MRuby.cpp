// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "MRuby.h"
#include "Utils.h"

namespace MRubyCSharp {
	namespace Interop {
		MRuby::MRuby() {
			this->state = mrb_open();
			this->InitCSharpModule();

			this->classList = gcnew SortedList<String^, MRubyClass^>();
		}
		MRuby::~MRuby() {
			if (this->classList != nullptr) {
				for each (MRubyClass^ klass in this->classList->Values) {
					delete klass;
				}
				this->classList = nullptr;
			}
			mrb_close(this->state);
		}

		Object^ MRuby::LoadString(String ^s)
		{
			marshal_context ctx;
			const char *code = ctx.marshal_as<const char*>(s);
			mrb_value value = mrb_load_string(this->state, code);

			return Utils::GetCSharpValue(this->state, value);
		}

		MRubyClass ^MRuby::CreateClass(Type ^type, String ^name)
		{
			return gcnew MRubyClass(this->state, this->csharpClass, type, name);
		}

		void MRuby::InitCSharpModule()
		{
			// Define CSharp module.
			this->csharpClass = mrb_define_module(this->state, "CSharp");

			// Add load method to CSharp.
			//MRubyFunc^ funcLoad = gcnew MRubyFunc(this, &MRuby::CSharpLoad);
			//GCHandle::Alloc(funcLoad);
			//IntPtr^ fpLoad = Marshal::GetFunctionPointerForDelegate(funcLoad);
			//mrb_define_module_function(this->state, this->csharpClass, "load", reinterpret_cast<mrb_func_t>(fpLoad->ToPointer()), MRB_ARGS_ANY());
		}

		mrb_value MRuby::CSharpLoad(mrb_state *state, mrb_value self)
		{
			mrb_value typeName;
			mrb_value name;

			mrb_get_args(state, "SS", &typeName, &name);

			String ^fullName = (String^)Utils::GetCSharpValue(state, typeName);

			Type ^type = nullptr;
			if (mrb_nil_p(name)) {
				type = Type::GetType(fullName);
			}
			else {
				System::Reflection::AssemblyName ^assemblyName = gcnew System::Reflection::AssemblyName((String^)Utils::GetCSharpValue(state, name));
				System::Reflection::Assembly ^assembly = AppDomain::CurrentDomain->Load(assemblyName);
				type = assembly->GetType(fullName);
			}

			if (type == nullptr) {
				return mrb_false_value();
			}

			MRubyClass^ klass = gcnew MRubyClass(state, this->csharpClass, type, nullptr);
			this->classList->Add(fullName, klass);

			return mrb_true_value();
		}
	}
}
