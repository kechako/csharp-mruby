#include "stdafx.h"
#include "MRubyClass.h"

namespace MRubyCSharp {
	namespace Interop {
		MRubyClass::MRubyClass(mrb_state *state, RClass* csharpModule, Type ^type, String ^name)
		{
			if (type == nullptr) {
				throw gcnew ArgumentNullException("type");
			}
			this->state = state;
			this->csharpModule = csharpModule;
			this->type = type;
			this->name = name;
			this->methods = gcnew SortedList<String^, MethodBase^>();

			this->DefineClass();
		}

		MRubyClass::~MRubyClass()
		{
			if (this->methods != nullptr) {
				for each (MethodBase ^method in this->methods->Values) {
					delete method;
				}
				this->methods = nullptr;
			}
			Marshal::FreeHGlobal(IntPtr((void*)this->dataType->struct_name));
			free(this->dataType);
		}

		void MRubyClass::Free(mrb_state *state, void *ptr)
		{
			InstanceHolder *holder = static_cast<InstanceHolder*>(ptr);
			delete holder;
		}

		void MRubyClass::DefineClass()
		{
			String ^name = this->name;
			if (String::IsNullOrWhiteSpace(name)) {
				name = type->Name;
			}

			this->dataType = (mrb_data_type*)malloc(sizeof(mrb_data_type));
			this->dataType->struct_name = (char *)Marshal::StringToHGlobalAnsi(this->name).ToPointer();
			this->funcDFree = gcnew DFree(this, &MRubyClass::Free);
			IntPtr ^fp = Marshal::GetFunctionPointerForDelegate(this->funcDFree);
			this->dataType->dfree = (void (*)(mrb_state *mrb, void*))fp->ToPointer();

			marshal_context ctx;
			this->klass = mrb_define_class_under(this->state, this->csharpModule, ctx.marshal_as<const char*>(name), this->state->object_class);
			MRB_SET_INSTANCE_TT(this->klass, MRB_TT_DATA);

			// Define methods
			this->DefineInitialize();
			this->DefineClassMethods();
			this->DefineInstanceMethods();
		}

		void MRubyClass::DefineInitialize()
		{
			InitializeMethod^ method = gcnew InitializeMethod(this, "initialize");
			this->methods->Add("initialize", method);
		}

		void MRubyClass::DefineClassMethods()
		{
			array<MethodInfo^>^ methods = type->GetMethods(BindingFlags::Static | BindingFlags::Public);

			for each (MethodInfo ^info in methods) {
				if (info->IsSpecialName) {
					continue;
				}

				if (this->methods->ContainsKey(info->Name)) {
					continue;
				}

				ClassMethod^ method = gcnew ClassMethod(this, info->Name);
				this->methods->Add(info->Name, method);
			}
		}

		void MRubyClass::DefineInstanceMethods()
		{
			array<MethodInfo^>^ methods = type->GetMethods(BindingFlags::Instance | BindingFlags::Public);

			for each (MethodInfo ^info in methods) {
				if (info->IsSpecialName) {
					continue;
				}

				if (this->methods->ContainsKey(info->Name)) {
					continue;
				}

				InstanceMethod^ method = gcnew InstanceMethod(this, info->Name);
				this->methods->Add(info->Name, method);
			}
		}

		void MRubyClass::DefineProperties()
		{
		}

		String ^MRubyClass::Name::get()
		{
			return this->name;
		}

		MRubyClass::MethodBase::MethodBase(MRubyClass ^klass, String^ name)
		{
			this->klass = klass;
			this->name = name;

			this->func = gcnew MRubyFunc(this, &MRubyClass::MethodBase::Func);
			IntPtr ^fp = Marshal::GetFunctionPointerForDelegate(this->func);

			this->DefineMethod(this->klass->state, this->klass->klass, reinterpret_cast<mrb_func_t>(fp->ToPointer()));
		}

		MRubyClass::MethodBase::~MethodBase()
		{
		}

		MRubyClass ^MRubyClass::MethodBase::Class::get()
		{
			return this->klass;
		}

		mrb_value MRubyClass::MethodBase::Func(mrb_state *state, mrb_value self)
		{
			array<Object^>^ param = gcnew array<Object^>(0);
			array<Type^>^ types = gcnew array<Type^>(0);

			mrb_value *args;
			mrb_int len;
			mrb_get_args(state, "*", &args, &len);
			param = gcnew array<Object^>(len);
			types = gcnew array<Type^>(len);

			for (int i = 0; i < len; i++) {
				param[i] = Utils::GetCSharpValue(state, args[i]);
				types[i] = param[i]->GetType();
			}

			return this->Func(state, self, this->klass->type, types, param);
		}

		MRubyClass::InitializeMethod::InitializeMethod(MRubyClass ^klass, String^ name) : MethodBase(klass, name)
		{
		}

		MRubyClass::InitializeMethod::~InitializeMethod()
		{
		}

		void MRubyClass::InitializeMethod::DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func)
		{
			marshal_context ctx;
			mrb_define_method(
				state,
				klass,
				ctx.marshal_as<const char*>(name),
				func,
				MRB_ARGS_ANY());
		}

		mrb_value MRubyClass::InitializeMethod::Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params)
		{
			DATA_TYPE(self) = this->Class->dataType;
			DATA_PTR(self) = NULL;

			ConstructorInfo^ info = type->GetConstructor(types);

			Object^ instance = info->Invoke(params);

			InstanceHolder *holder = new InstanceHolder();
			holder->instance = instance;

			DATA_PTR(self) = holder;

			return self;
		}

		MRubyClass::ClassMethod::ClassMethod(MRubyClass ^klass, String^ name) : MethodBase(klass, name)
		{
		}

		MRubyClass::ClassMethod::~ClassMethod()
		{
		}

		void MRubyClass::ClassMethod::DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func)
		{
			marshal_context ctx;
			mrb_define_class_method(
				state,
				klass,
				ctx.marshal_as<const char*>(name),
				func,
				MRB_ARGS_ANY());
		}

		mrb_value MRubyClass::ClassMethod::Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params)
		{
			MethodInfo^ method = type->GetMethod(this->name, types);

			Object ^result = method->Invoke(nullptr, params);

			return Utils::GetMRubyValue(state, result);
		}

		MRubyClass::InstanceMethod::InstanceMethod(MRubyClass ^klass, String^ name) : MethodBase(klass, name)
		{
		}

		MRubyClass::InstanceMethod::~InstanceMethod()
		{
		}

		void MRubyClass::InstanceMethod::DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func)
		{
			marshal_context ctx;
			mrb_define_method(
				state,
				klass,
				ctx.marshal_as<const char*>(name),
				func,
				MRB_ARGS_ANY());
		}

		mrb_value MRubyClass::InstanceMethod::Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params)
		{
			InstanceHolder *holder = static_cast<InstanceHolder*>(mrb_get_datatype(state, self, this->Class->dataType));

			Object^ instance = holder->instance;

			MethodInfo^ method = type->GetMethod(this->name, types);

			Object ^result = method->Invoke(instance, params);

			return Utils::GetMRubyValue(state, result);
		}
	}
}
