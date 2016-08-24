#pragma once

#include <msclr\marshal.h>
#include <mruby.h>
#include <mruby\compile.h>
#include <mruby\data.h>

#include "Utils.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace msclr::interop;

namespace MRubyCSharp {
	namespace Interop {
		public delegate mrb_value MRubyFunc(mrb_state* state, mrb_value self);
		public delegate void DFree(mrb_state *state, void* ptr);

		class InstanceHolder {
		public:
			gcroot<Object^> instance;
		};

		public ref class MRubyClass
		{
			ref class MethodBase abstract
			{
			protected:
				MethodBase(MRubyClass ^klass, String ^name);
				~MethodBase();

			protected:
				virtual void DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func) abstract;
				virtual mrb_value Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params) abstract;

			private:
				mrb_value Func(mrb_state *state, mrb_value self);

			protected:
				property MRubyClass^ Class { MRubyClass ^get(); }

			protected:
				String ^name;
			private:
				MRubyClass ^klass;
				IntPtr gcHandle;

			};

			ref class InitializeMethod : MethodBase
			{
			public:
				InitializeMethod(MRubyClass ^klass, String ^name);
				~InitializeMethod();
			protected:
				virtual void DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func) override;
				virtual mrb_value Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params) override;
			};

			ref class ClassMethod : MethodBase
			{
			public:
				ClassMethod(MRubyClass ^klass, String ^name);
				~ClassMethod();
			protected:
				virtual void DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func) override;
				virtual mrb_value Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params) override;

			};
			ref class InstanceMethod : MethodBase
			{
			public:
				InstanceMethod(MRubyClass ^klass, String ^name);
				~InstanceMethod();
			protected:
				virtual void DefineMethod(mrb_state *state, RClass *klass, mrb_func_t func) override;
				virtual mrb_value Func(mrb_state *state, mrb_value self, Type ^type, array<Type^>^ types, array<Object^>^ params) override;
			};

		internal:
			MRubyClass(mrb_state *state, RClass *csharpModule, Type ^type, String ^name);
			~MRubyClass();

		private:
			void Free(mrb_state *state, void* ptr);
			void DefineClass();
			void DefineInitialize();
			void DefineInstanceMethods();
			void DefineClassMethods();
			void DefineProperties();

		private:
			mrb_state *state;
			RClass *csharpModule;
			RClass *klass;
			Type ^type;
			String ^name;
			SortedList<String^, MethodBase^>^ methods;
			mrb_data_type *dataType;
			IntPtr gcHandle;

		public:
			property String^ Name { String ^get(); }
		};
	}
}