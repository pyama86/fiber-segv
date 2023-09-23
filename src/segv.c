#include "mruby.h"
#include "mruby/variable.h"
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/proc.h>
#include <mruby/internal.h>
#include <stdio.h>
void p(mrb_state *mrb,mrb_value value) {
    printf("--> %s\n", mrb_str_to_cstr(mrb, mrb_inspect(mrb, value)));
}
static mrb_value mrb_segv_init(mrb_state *mrb, mrb_value self)
{
    mrb_sym new = mrb_intern_cstr(mrb, "new");
    char code1[] = "3.times { puts 'Hello, World1';Fiber.yield(); }";
    char code2[] = "3.times { puts 'Hello, World2';Fiber.yield(); }";

    mrbc_context *cxt1 = mrbc_context_new(mrb);
    mrbc_context *cxt2 = mrbc_context_new(mrb);

    mrb_value str1 = mrb_str_new_cstr(mrb, code1);
    mrb_value str2 = mrb_str_new_cstr(mrb, code2);

    mrb_value fiber_class1 = mrb_obj_value(mrb_class_get(mrb, "Fiber"));
    mrb_value fiber_class2 = mrb_obj_value(mrb_class_get(mrb, "Fiber"));

    struct mrb_parser_state *p1 = mrb_parse_string(mrb, RSTRING_PTR(str1), cxt1);
    struct mrb_parser_state *p2 = mrb_parse_string(mrb, RSTRING_PTR(str2), cxt2);

    struct RProc *proc1 = mrb_generate_code(mrb, p1);
    mrb_value fiber1 = mrb_funcall_with_block(mrb, fiber_class1, new, 0, NULL, mrb_obj_value(mrb_closure_new(mrb, proc1->body.irep)));
    p(mrb, mrb_fiber_resume(mrb, fiber1, 0, NULL));
    p(mrb, mrb_fiber_resume(mrb, fiber1, 0, NULL));
    p(mrb, mrb_fiber_resume(mrb, fiber1, 0, NULL));

    struct RProc *proc2 = mrb_generate_code(mrb, p2);
    mrb_value fiber2 = mrb_funcall_with_block(mrb, fiber_class2, new, 0, NULL, mrb_obj_value(mrb_closure_new(mrb, proc2->body.irep)));
    p(mrb, mrb_fiber_resume(mrb, fiber2, 0, NULL));
    p(mrb, mrb_fiber_resume(mrb, fiber2, 0, NULL));
    p(mrb, mrb_fiber_resume(mrb, fiber2, 0, NULL));
}

void mrb_fiber_segv_gem_init(mrb_state *mrb)
{
  struct RClass *segv;

  segv = mrb_define_class(mrb, "Segv", mrb->object_class);
  mrb_define_method(mrb, segv, "initialize", mrb_segv_init, MRB_ARGS_NONE());
  mrb_gc_arena_restore(mrb, 0);

}

void mrb_fiber_segv_gem_final(mrb_state *mrb)
{
}
