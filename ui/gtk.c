#include <stdio.h>
#include <gtk/gtk.h>
#include "internal.h"
#include "rccnames.h"

#define TITLE_WIDTH 160
#define TITLE_HEIGHT 10
#define BOX_SPACING 1
#define BOX_BORDER 0
#define FRAME_SPACING 1
#define FRAME_BORDER 0
#define PAGE_SPACING 0

rcc_ui_internal rccUiCreateInternal(rcc_ui_context ctx) {
    return NULL;
}

void rccUiFreeInternal(rcc_ui_context ctx) {
}

rcc_ui_widget rccUiMenuCreateWidget(rcc_ui_menu_context ctx) {
    return NULL;
}

void rccUiMenuFreeWidget(rcc_ui_menu_context ctx) {
}

rcc_ui_id rccUiMenuGet(rcc_ui_menu_context ctx) {
    GtkWidget *menu;
    
    if (!ctx) return (rcc_ui_id)-1;

    if (ctx->type == RCC_UI_MENU_OPTION)
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ctx->widget));
    
    menu = gtk_option_menu_get_menu(ctx->widget);
    return g_list_index(GTK_MENU_SHELL(menu)->children, gtk_menu_get_active(GTK_MENU(menu)));
}

int rccUiMenuSet(rcc_ui_menu_context ctx, rcc_ui_id id) {
    if (!ctx) return -1;
    
    switch (ctx->type) {
	case RCC_UI_MENU_OPTION:
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ctx->widget),id);
	break;
	default:
	    gtk_option_menu_set_history(ctx->widget, id);
    }

    return 0;
}


static int rccGtkMenuLanguageCB(GtkWidget * w, gpointer item) {
    rccUiRestoreLanguage(((rcc_ui_menu_context)item)->uictx);
}

int rccUiMenuConfigureWidget(rcc_ui_menu_context ctx) {
    unsigned int i;
    
    rcc_context rccctx;
    rcc_ui_context uictx;

    rcc_language_config config;
    rcc_language_ptr *languages;
    rcc_language_id language_id;

    rcc_charset *charsets;
    rcc_engine_ptr *engines;

    rcc_charset_id charset_id;
    rcc_engine_id engine_id;

    GtkWidget *list, *item, *menu;

    if (!ctx) return -1;
    
    uictx = ctx->uictx;
    rccctx = uictx->rccctx;
    
    switch (ctx->type) {
	case RCC_UI_MENU_LANGUAGE:
	    list = gtk_menu_new();

	    languages=rccGetLanguageList(rccctx);
	    for (i=0; languages[i]; i++) {
		item = gtk_menu_item_new_with_label(rccUiGetLanguageName(uictx, languages[i]->sn));
		gtk_signal_connect(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(rccGtkMenuLanguageCB), ctx);
		gtk_menu_append(GTK_MENU(list), item);
		gtk_widget_show(item);
	    }

	    language_id = rccGetSelectedLanguage(rccctx);
	    if (language_id < 0) language_id = 0;

	    if (ctx->widget) menu = ctx->widget;
	    else {
		menu = gtk_option_menu_new();
		ctx->widget = menu;
	    }
	    gtk_option_menu_remove_menu(GTK_OPTION_MENU(menu));
	    gtk_option_menu_set_menu(GTK_OPTION_MENU(menu), list);
	    gtk_option_menu_set_history(GTK_OPTION_MENU(menu), language_id);
	break;
	case RCC_UI_MENU_CHARSET:
	    list = gtk_menu_new();

	    language_id = (rcc_language_id)rccUiMenuGet(uictx->language);
	    charsets = rccGetCharsetList(rccctx, language_id);
	    
	    for (i=0;charsets[i];i++) {
		item = gtk_menu_item_new_with_label(charsets[i]);
		gtk_widget_show(item);
    		gtk_menu_append(GTK_MENU(list), item);
		puts(charsets[i]);
	    }

	    if (ctx->widget) menu = ctx->widget;
	    else {
		menu = gtk_option_menu_new();
		ctx->widget = menu;
	    }
    
	    gtk_option_menu_remove_menu(GTK_OPTION_MENU(menu));
	    gtk_option_menu_set_menu(GTK_OPTION_MENU(menu), list);

	    config = rccGetConfig(rccctx, language_id);
	    charset_id = rccConfigGetSelectedCharset(config, (rcc_class_id)ctx->id);
	    if (charset_id < 0) charset_id = 0;
	    gtk_option_menu_set_history(GTK_OPTION_MENU(menu), charset_id);
	break;
	case RCC_UI_MENU_ENGINE:
	    list = gtk_menu_new();

	    language_id = (rcc_language_id)rccUiMenuGet(uictx->language);
	    engines = rccGetEngineList(rccctx, language_id);
	    for (i=0;engines[i];i++) {
		item = gtk_menu_item_new_with_label(engines[i]->title);
		gtk_widget_show(item);
    		gtk_menu_append(GTK_MENU(list), item);
		puts(engines[i]->title);
	    }	

	    if (ctx->widget) menu = ctx->widget;
	    else {
		menu = gtk_option_menu_new();
		ctx->widget = menu;
	    }

	    gtk_option_menu_remove_menu(GTK_OPTION_MENU(menu));
	    gtk_option_menu_set_menu(GTK_OPTION_MENU(menu), list);

	    config = rccGetConfig(rccctx, language_id);
	    engine_id = rccConfigGetSelectedEngine(config);
	    if (engine_id < 0) engine_id = 0;
	    gtk_option_menu_set_history(GTK_OPTION_MENU(menu), engine_id);
		    
	break;
	case RCC_UI_MENU_OPTION:
	    if (!ctx->widget) {
	        item = gtk_check_button_new_with_label(rccUiGetOptionName(uictx, (rcc_option)ctx->id));
    		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(item), rccGetOption(rccctx, (rcc_option)ctx->id));
		ctx->widget = item;
	    }
	break;
    }

    return 0;
}


rcc_ui_box rccUiBoxCreate(rcc_ui_menu_context ctx, const char *title) {
    GtkWidget *hbox, *label;
    hbox = gtk_hbox_new(FALSE, BOX_SPACING);
    gtk_container_border_width(GTK_CONTAINER(hbox), BOX_BORDER);
    if (ctx->type != RCC_UI_MENU_OPTION) {
	label = gtk_label_new(title);
	gtk_widget_set_usize(label, TITLE_WIDTH, TITLE_HEIGHT);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    }
    gtk_box_pack_start(GTK_BOX(hbox), (GtkWidget*)ctx->widget, TRUE, TRUE, 0);
    return (rcc_ui_box)hbox;
}

rcc_ui_frame rccUiFrameCreate(rcc_ui_frame_context ctx, const char *title) {
    GtkWidget *frame, *box;

    if (!ctx) return NULL;
    
    frame = gtk_frame_new(title?title:"");
    gtk_container_border_width(GTK_CONTAINER(frame), FRAME_BORDER);

    box = gtk_vbox_new(FALSE, FRAME_SPACING);
    gtk_container_add(GTK_CONTAINER(frame), box);
    
    return (rcc_ui_frame)frame;
}

void rccUiFrameFree(rcc_ui_frame_context ctx) {
}


int rccUiFrameAdd(rcc_ui_frame_context ctx, rcc_ui_box box) {
    GtkWidget *vbox;
    
    if ((!ctx)||(!box)) return -1;
    
    vbox = gtk_container_children(GTK_CONTAINER(ctx->frame))->data;
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(box), FALSE, FALSE, 0);
    return 0;
}

rcc_ui_page rccUiPageCreate(rcc_ui_context ctx, const char *title) {
    return (rcc_ui_page)gtk_vbox_new(FALSE, PAGE_SPACING);
}

int rccUiPageAdd(rcc_ui_page page, rcc_ui_frame frame) {
    if ((!page)||(!frame)) return -1;
    gtk_box_pack_start(GTK_BOX(page), GTK_WIDGET(frame), FALSE, FALSE, 0);
    return 0;
}
