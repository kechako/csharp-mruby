// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "MRuby.h"
#include "Utils.h"

namespace MRubyCSharp {
	namespace Interop {
		MRuby::MRuby(String ^ moduleName) {
			this->moduleName = moduleName;
			this->mrb = mrb_open();
			this->InitCSharpModule();

			this->assemblyList = gcnew List<Assembly^>();
			this->classList = gcnew SortedList<String^, MRubyClass^>();
		}
		MRuby::~MRuby() {
			if (this->classList != nullptr) {
				for each (MRubyClass^ klass in this->classList->Values) {
					delete klass;
				}
				this->classList = nullptr;
			}
			mrb_close(this->mrb);
		}

		Object^ MRuby::LoadString(String ^s)
		{
			marshal_context ctx;
			const char *code = ctx.marshal_as<const char*>(s);

			mrb_value value = mrb_load_string(this->mrb, code);

			Object ^ result = Utils::GetCSharpValue(this->mrb, value);

			return result;
		}

		MRubyClass ^MRuby::CreateClass(Type ^type, String ^name)
		{
			return gcnew MRubyClass(this->mrb, this->csharpClass, type, name);
		}

		void MRuby::InitCSharpModule()
		{
			// Define CSharp module.
			marshal_context ctx;
			this->csharpClass = mrb_define_module(this->mrb, ctx.marshal_as<const char*>(this->moduleName));

			// test メソッド
			this->funcTest = gcnew MRubyFunc(this, &MRuby::CSharpTest);
			IntPtr^ fpTest = Marshal::GetFunctionPointerForDelegate(this->funcTest);
			mrb_define_module_function(this->mrb, this->csharpClass, "msg_box", static_cast<mrb_func_t>(fpTest->ToPointer()), MRB_ARGS_REQ(1));

			// load メソッド
			this->funcLoad = gcnew MRubyFunc(this, &MRuby::CSharpLoad);
			IntPtr^ fpLoad = Marshal::GetFunctionPointerForDelegate(this->funcLoad);
			mrb_define_module_function(this->mrb, this->csharpClass, "load", static_cast<mrb_func_t>(fpLoad->ToPointer()), MRB_ARGS_REQ(1));

			// using メソッド
			this->funcUsing = gcnew MRubyFunc(this, &MRuby::CSharpUsing);
			IntPtr^ fpUsing = Marshal::GetFunctionPointerForDelegate(this->funcUsing);
			mrb_define_module_function(this->mrb, this->csharpClass, "using", static_cast<mrb_func_t>(fpUsing->ToPointer()), MRB_ARGS_REQ(1));
		}

		mrb_value MRuby::CSharpTest(mrb_state *mrb, mrb_value self)
		{
			mrb_value text;

			mrb_get_args(mrb, "S", &text);

//			String ^msg = (String^)Utils::GetCSharpValue(mrb, text);

//			System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show(msg, "MRuby", System::Windows::Forms::MessageBoxButtons::YesNo, System::Windows::Forms::MessageBoxIcon::Question);

//			return Utils::GetMRubyValue(mrb, result.ToString());
			return text;
		}

		mrb_value MRuby::CSharpLoad(mrb_state *mrb, mrb_value self)
		{
			mrb_value name;

			mrb_get_args(mrb, "S", &name);

			AssemblyName ^assemblyName = gcnew AssemblyName((String^)Utils::GetCSharpValue(mrb, name));
			Assembly ^assembly = AppDomain::CurrentDomain->Load(assemblyName);

			this->assemblyList->Add(assembly);

			return mrb_true_value();
		}

		mrb_value MRuby::CSharpUsing(mrb_state *mrb, mrb_value self)
		{
			mrb_value name;

			mrb_get_args(mrb, "S", &name);

			String ^fullName = (String^)Utils::GetCSharpValue(mrb, name);

			Type ^type = Type::GetType(fullName);
			if (type == nullptr) {
				for each (Assembly ^ assembly in this->assemblyList) {
					type = assembly->GetType(fullName);
					if (type != nullptr) {
						break;
					}
				}
			}

			if (type == nullptr) {
				return mrb_false_value();
			}

			MRubyClass^ klass = gcnew MRubyClass(mrb, this->csharpClass, type, nullptr);
			this->classList->Add(fullName, klass);

			return mrb_true_value();
		}
	}
}
