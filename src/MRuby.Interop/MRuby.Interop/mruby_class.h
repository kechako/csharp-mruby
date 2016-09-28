#pragma once

#include <vcclr.h>
#include <mruby.h>
#include <mruby\data.h>
#include "MRubyClass.h"

using namespace System;
using namespace MRubyCSharp::Interop;

class mruby_class
{
public:
	mruby_class(mrb_state *mrb, const char *name, RClass *super, Type ^type);
	~mruby_class();

private:
	static mrb_value initialize(mrb_state * mrb, mrb_value self);
	static mrb_value instance_method(mrb_state * mrb, mrb_value self);
	static void class_free(mrb_state *mrb, void *ptr);

public:
	gcroot<MRubyClass> mrubyClass;

private:
	mrb_state *mrb;
	RClass *rclass;
	struct mrb_data_type *data_type;
};

