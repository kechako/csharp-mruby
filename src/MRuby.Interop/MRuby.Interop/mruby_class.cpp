#include "stdafx.h"
#include "mruby_class.h"

mruby_class::mruby_class(mrb_state *mrb, const char *name, RClass *super, Type ^type)
{
	this->mrb = mrb;

	this->data_type = (struct mrb_data_type *)malloc(sizeof(struct mrb_data_type));
	this->data_type->struct_name = name;
	this->data_type->dfree = mruby_class::class_free;

	this->rclass = mrb_define_class(this->mrb, name, super);
	mrb_define_method(this->mrb, this->rclass, "initialize", mruby_class::initialize, MRB_ARGS_NONE());
}


mruby_class::~mruby_class()
{
	free(this->data_type);
}

mrb_value mruby_class::initialize(mrb_state *mrb, mrb_value self)
{
}

mrb_value mruby_class::instance_method(mrb_state *mrb, mrb_value self)
{
}

void mruby_class::class_free(mrb_state *mrb, void *ptr) {
	mruby_class *klass = static_cast<mruby_class*>(ptr);
	delete klass;
}