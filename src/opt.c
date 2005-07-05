#include <stdio.h>

#include "internal.h"
#include "opt.h"

rcc_option_value rccGetOption(rcc_context ctx, rcc_option option) {
    if ((!ctx)||(option<0)||(option>=RCC_MAX_OPTIONS)) return -1;
    
    return ctx->options[option];
}

int rccOptionIsDefault(rcc_context ctx, rcc_option option) {
    if ((!ctx)||(option<0)||(option>=RCC_MAX_OPTIONS)) return -1;

    return ctx->default_options[option];
}

int rccSetOption(rcc_context ctx, rcc_option option, rcc_option_value value) {
    if ((!ctx)||(option>=RCC_MAX_OPTIONS)) return -1;

    ctx->default_options[option] = 0;

    if (ctx->options[option] != value) {
	ctx->configure = 1;
	ctx->options[option]=value;
    }
    
    return 0;
}

int rccOptionSetDefault(rcc_context ctx, rcc_option option) {
    rcc_option_value value;
    if ((!ctx)||(option>=RCC_MAX_OPTIONS)) return -1;

    ctx->default_options[option] = 1;
    value = rccGetOptionDefaultValue(option);

    if (ctx->options[option] != value) {
	ctx->configure = 1;
	ctx->options[option]=value;
    }
    
    return 0;
}
