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
    int found_a;
    int found_b;
};

void notified_function(hook_res* res, NotifyDetourAcceptor::call_state state)
{
    res->found_a = (int)state.rcx;
    res->found_b = (int)state.rdx;
}

int main()
{
    hook_res res;
    res.found_a = 0;
    res.found_b = 0;

    int test_a = 3;
    int test_b = 4;

    auto fn_ptr = &notified_function;

    fn_hook* h = new fn_hook((void*)&add_fn,(void*&)fn_ptr, (void*)&res);

    h->Enable();
    int result = add_fn(test_a,test_b);
    std::cout << "Hello, World! Result of addition is: " << result << std::endl;

    std::cout << "Found a: " << res.found_a << " ; b: " << res.found_b << std::endl;
    h->Disable();

    delete h;

    if (res.found_a == test_a && res.found_b == test_b) {
        return 0;
    }
    
    return 1;
}