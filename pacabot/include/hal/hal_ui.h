/*---------------------------------------------------------------------------
 *
 *      hal_ui.h
 *
 *---------------------------------------------------------------------------*/

#ifndef __HAL_UI_H__
#define __HAL_UI_H__

/* Module Identifier */
#define HAL_UI_MODULE_ID    13

/* Error codes */
#define HAL_UI_E_SUCCESS    0
#define HAL_UI_E_ERROR      MAKE_ERROR(HAL_UI_MODULE_ID, 1)
#define HAL_UI_E_BAD_HANDLE MAKE_ERROR(HAL_UI_MODULE_ID, 2)

/* Maximum number of items per page */
#define MAX_PAGE_ITEMS      5

#define MENU_DIRECTION_UP        1
#define MENU_DIRECTION_RIGHT     2
#define MENU_DIRECTION_DOWN      3
#define MENU_DIRECTION_LEFT      4

/* HAL UI handle */
typedef void *HAL_UI_HANDLE;

/* Menu item definition */
typedef struct
{
    char *str;              // String representing the item in a menu
    int (*handler)(void);   // Pointer toward the function called by the item
    void *param;            // Pointer toward the parameter to modify (if any)
    int step;               // Step increment
    unsigned char *icon;    // Icon of the menu item
} menu_item;

/* BMP menu item definition */
typedef struct
{
    int (*handler)(void *);
    const unsigned char *bmp;
    const unsigned char *bmp_hover;
    unsigned char x;
    unsigned char y;
    unsigned char w;
    unsigned char h;
    unsigned int  select_direction;
    const char *str;
} bmp_menu_item;

/* Exported functions */
int hal_ui_init(void);
int hal_ui_terminate(void);
int hal_ui_open(HAL_UI_HANDLE *handle, void *params);
int hal_ui_close(HAL_UI_HANDLE handle);

int hal_ui_refresh(HAL_UI_HANDLE handle);
int hal_ui_clear_scr(HAL_UI_HANDLE handle);
int hal_ui_display_txt(HAL_UI_HANDLE handle, int x, int y, const char *str);
int hal_ui_display_prompt(HAL_UI_HANDLE handle,
                          const char *title, const char *str);
int hal_ui_display_menu(HAL_UI_HANDLE handle, void *menu,
                        int *selected_index, unsigned char exit_on_select);
int hal_ui_display_bmp_menu(HAL_UI_HANDLE handle, void *menu, void *param);
int hal_ui_modify_bool_param(HAL_UI_HANDLE handle,
                             char *param_name, unsigned char *param);
int hal_ui_modify_long_param(HAL_UI_HANDLE handle, char *param_name,
                             unsigned long *param, int step);
int hal_ui_modify_int_param(HAL_UI_HANDLE handle, char *param_name,
                             unsigned int *param, int step);
int hal_ui_draw_line(HAL_UI_HANDLE handle,
                     unsigned char x0, unsigned char y0,
                     unsigned char x1, unsigned char y1);
int hal_ui_fill_rect(HAL_UI_HANDLE handle,
                     unsigned char x, unsigned char y,
                     unsigned char w, unsigned char h);
int hal_ui_fill_circle(HAL_UI_HANDLE handle,
                       unsigned char x0, unsigned char y0,
                       unsigned char r);
//void hal_ui_anti_rebonds (uint_fast16_t* gpio, uint16_t gpio_pin);

#endif // __HAL_UI_H__
