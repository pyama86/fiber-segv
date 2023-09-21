refs: https://github.com/mruby/mruby/issues/6060

Creating an mrb state should not be a problem, but it seems like it won't work if reused.

# usage
```
$ make run
```


## It is OK
```c
static mrb_value mrb_segv_init(mrb_state *mrb, mrb_value self)
{
    mrb_state *mrb1 = mrb_open();
    mrb_sym new = mrb_intern_cstr(mrb1, "new");
    char code[] = "puts 'Hello, World!'";

    mrbc_context *cxt1 = mrbc_context_new(mrb1);

    mrb_value str1 = mrb_str_new_cstr(mrb1, code);

    mrb_value fiber_class1 = mrb_obj_value(mrb_class_get(mrb1, "Fiber"));

    struct mrb_parser_state *p1 = mrb_parse_string(mrb1, RSTRING_PTR(str1), cxt1);

    struct RProc *proc1 = mrb_generate_code(mrb1, p1);


    mrb_value fiber1 = mrb_funcall_with_block(mrb1, fiber_class1, new, 0, NULL, mrb_obj_value(mrb_closure_new(mrb1, proc1->body.irep)));
    p(mrb1, mrb_fiber_resume(mrb1, fiber1, 0, NULL));
}
```

## It is NG
```c
static mrb_value mrb_segv_init(mrb_state *mrb, mrb_value self)
{
    mrb_sym new = mrb_intern_cstr(mrb, "new");
    char code[] = "puts 'Hello, World!'";

    mrbc_context *cxt1 = mrbc_context_new(mrb);

    mrb_value str1 = mrb_str_new_cstr(mrb, code);

    mrb_value fiber_class1 = mrb_obj_value(mrb_class_get(mrb, "Fiber"));

    struct mrb_parser_state *p1 = mrb_parse_string(mrb, RSTRING_PTR(str1), cxt1);

    struct RProc *proc1 = mrb_generate_code(mrb, p1);


    mrb_value fiber1 = mrb_funcall_with_block(mrb, fiber_class1, new, 0, NULL, mrb_obj_value(mrb_closure_new(mrb, proc1->body.irep)));
    p(mrb, mrb_fiber_resume(mrb, fiber1, 0, NULL));
}
```
