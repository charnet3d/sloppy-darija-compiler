#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <setjmp.h>
#include <intrin.h>

#define MAX_EXCEPTION_DEPTH 100

struct ExceptionContext {
    CONTEXT ctx;
    bool active;
    int ret_val;
};

static ExceptionContext g_context_stack[MAX_EXCEPTION_DEPTH];
static int g_context_depth = 0;
static void* g_current_exception = nullptr;

extern "C" {
    void print_wrapper(void* val, int len) {
        static int initialized = 0;
        if (!initialized) {
            SetConsoleOutputCP(CP_UTF8);
            initialized = 1;
        }
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD bytesWritten;
        WriteFile(hStdOut, val, len, &bytesWritten, NULL);
        
        char newline = '\n';
        WriteFile(hStdOut, &newline, 1, &bytesWritten, NULL);
    }

    int runtime_setjmp() {
        if (g_context_depth >= MAX_EXCEPTION_DEPTH) {
            printf("Runtime Error: Exception depth exceeded\n");
            ExitProcess(1);
        }
        ExceptionContext& ctx = g_context_stack[g_context_depth];
        ctx.active = true;
        ctx.ret_val = 0;
        g_context_depth++;
        RtlCaptureContext(&ctx.ctx);
        return ctx.ret_val;
    }

    void runtime_push_context() {
        g_context_depth++;
    }

    void runtime_pop_context() {
        if (g_context_depth > 0) {
            g_context_depth--;
        }
    }

    void runtime_throw(void* obj) {
        g_current_exception = obj;
        if (g_context_depth <= 0) {
            printf("Unhandled Exception: %p\n", obj);
            ExitProcess(1);
        }
        g_context_depth--;
        ExceptionContext& ctx = g_context_stack[g_context_depth];
        ctx.ret_val = 1;
        RtlRestoreContext(&ctx.ctx, NULL);
    }

    void* runtime_get_exception() {
        return g_current_exception;
    }

 
}
