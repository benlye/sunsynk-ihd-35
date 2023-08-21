SET(SOURCES screens/ui_home.c
    screens/ui_plot.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_1549501229.c
    images/ui_img_670870020.c
    images/ui_img_705529016.c
    images/ui_img_264836261.c
    images/ui_img_1076282785.c
    images/ui_img_1230882528.c
    images/ui_img_1765460539.c
    images/ui_img_1683162281.c
    images/ui_img_1288183160.c
    images/ui_img_811431924.c
    images/ui_img_226794803.c
    images/ui_img_207095971.c
    images/ui_img_1192762099.c
    fonts/ui_font_mont_64.c)

add_library(ui ${SOURCES})
