#include <hook_any_x64/fn_hook.h>
#include <hook_any_x64/notifydetouracceptor.h>

#include <iostream>
#include <string>

int add_fn(int a, int b)
{
    return a + b;
}

struct hook_res
{
    int alter_a;
    int alter_b;
};

void notified_function(hook_res* res, NotifyDetourAcceptor::call_state* state)
{
    state->rcx = (uint64_t)res->alter_a;
    state->rdx = (uint64_t)res->alter_b;
}

int main()
{
    hook_res res;
    res.alter_a = 1330;
    res.alter_b = 7;

    int test_a = 3;
    int test_b = 4;

    auto fn_ptr = &notified_function;

    fn_hook* h = new fn_hook((void*)&add_fn,(void*&)fn_ptr, (void*)&res);

    h->Enable();
    int result = add_fn(test_a,test_b);
    h->Disable();

    delete h;

    if (result == res.alter_a + res.alter_b) {
        return 0;
    }
    
    return 1;
}
