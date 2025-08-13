#include "entertainment.h"

#include "building/count.h"
#include "city/culture.h"
#include "city/entertainment.h"
#include "city/festival.h"
#include "city/gods.h"
#include "city/houses.h"
#include "graphics/generic_button.h"
#include "graphics/image.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/text.h"
#include "graphics/window.h"
#include "window/hold_festival.h"

#define ADVISOR_HEIGHT 23

#define PEOPLE_OFFSET 330
#define COVERAGE_OFFSET 470
#define COVERAGE_WIDTH 130

static void button_hold_festival(int param1, int param2);

static generic_button hold_festival_button[] = {
    {102, 280, 300, 20, button_hold_festival, button_none, 0, 0},
};

static int focus_button_id;

static int get_entertainment_advice(void)
{
    const house_demands *demands = city_houses_demands();
    if (demands->missing.entertainment > demands->missing.more_entertainment) {
        return 3;
    } else if (!demands->missing.more_entertainment) {
        return city_culture_average_entertainment() ? 1 : 0;
    } else if (city_entertainment_venue_needing_shows()) {
        return 3 + city_entertainment_venue_needing_shows();
    } else {
        return 2;
    }
}

static int get_festival_advice(void)
{
    int months_since_festival = city_festival_months_since_last();
    if (months_since_festival <= 1) {
        return 0;
    } else if (months_since_festival <= 6) {
        return 1;
    } else if (months_since_festival <= 12) {
        return 2;
    } else if (months_since_festival <= 18) {
        return 3;
    } else if (months_since_festival <= 24) {
        return 4;
    } else if (months_since_festival <= 30) {
        return 5;
    } else {
        return 6;
    }
}

static void draw_festival_info(void)
{
    inner_panel_draw(48, 252, 34, 6);
    image_draw(image_group(GROUP_PANEL_WINDOWS) + 15, 460, 255);
    lang_text_draw(58, 17, 52, 224, FONT_LARGE_BLACK);

    int width = lang_text_draw_amount(8, 4, city_festival_months_since_last(), 112, 260, FONT_NORMAL_WHITE);
    lang_text_draw(58, 15, 112 + width, 260, FONT_NORMAL_WHITE);
    if (city_festival_is_planned()) {
        lang_text_draw_centered(58, 34, 102, 284, 300, FONT_NORMAL_WHITE);
    } else {
        lang_text_draw_centered(58, 16, 102, 284, 300, FONT_NORMAL_WHITE);
    }
    lang_text_draw_multiline(58, 18 + get_festival_advice(), 56, 305, 400, FONT_NORMAL_WHITE);
}

static void draw_building_row(int y_offset, building_type building, 
    int building_text_id, int building_coverage, int shows, int pct_coverage)
{
    lang_text_draw_amount(8, building_text_id, building_count_total(building), 40, y_offset, FONT_NORMAL_WHITE);
    text_draw_number_centered(building_count_active(building), 150, y_offset, 100, FONT_NORMAL_WHITE);
    text_draw_number_centered(shows, 230, y_offset, 100, FONT_NORMAL_WHITE);
    int width = text_draw_number(building_coverage * building_count_active(building), '@', " ",
        PEOPLE_OFFSET, y_offset, FONT_NORMAL_WHITE);
    lang_text_draw(58, 5, PEOPLE_OFFSET + width, y_offset, FONT_NORMAL_WHITE);
    if (pct_coverage == 0) {
        lang_text_draw_centered(57, 10, COVERAGE_OFFSET, y_offset, COVERAGE_WIDTH, FONT_NORMAL_WHITE);
    } else if (pct_coverage < 100) {
        lang_text_draw_centered(57, 11 + pct_coverage / 10, 
            COVERAGE_OFFSET, y_offset, COVERAGE_WIDTH, FONT_NORMAL_WHITE);
    } else {
        lang_text_draw_centered(57, 21, COVERAGE_OFFSET, y_offset, COVERAGE_WIDTH, FONT_NORMAL_WHITE);
    }
}

static void draw_hippodrome_row(int y_offset)
{
    lang_text_draw_amount(8, 40, building_count_total(BUILDING_HIPPODROME), 40, y_offset, FONT_NORMAL_WHITE);
    text_draw_number_centered(building_count_active(BUILDING_HIPPODROME), 150, y_offset, 100, FONT_NORMAL_WHITE);
    text_draw_number_centered(city_entertainment_hippodrome_shows(), 230, y_offset, 100, FONT_NORMAL_WHITE);
    lang_text_draw_centered(58, 6, PEOPLE_OFFSET + 10, y_offset, 100, FONT_NORMAL_WHITE);
    if (city_culture_coverage_hippodrome() == 0) {
        lang_text_draw_centered(57, 10, COVERAGE_OFFSET, y_offset, COVERAGE_WIDTH, FONT_NORMAL_WHITE);
    } else {
        lang_text_draw_centered(57, 21, COVERAGE_OFFSET, y_offset, COVERAGE_WIDTH, FONT_NORMAL_WHITE);
    }
}

static int draw_background(void)
{
    city_gods_calculate_moods(0);
    city_culture_calculate();

    outer_panel_draw(0, 0, 40, ADVISOR_HEIGHT);
    image_draw(image_group(GROUP_ADVISOR_ICONS) + 8, 10, 10);

    lang_text_draw(58, 0, 60, 12, FONT_LARGE_BLACK);

    lang_text_draw(58, 1, 180, 46, FONT_SMALL_PLAIN);
    lang_text_draw(58, 2, 260, 46, FONT_SMALL_PLAIN);
    lang_text_draw(58, 3, PEOPLE_OFFSET + 10, 46, FONT_SMALL_PLAIN);
    lang_text_draw_centered(58, 4, COVERAGE_OFFSET, 46, COVERAGE_WIDTH, FONT_SMALL_PLAIN);

    inner_panel_draw(32, 60, 36, 5);

    int theater_shows = city_entertainment_theater_shows();
    int amphitheater_shows = city_entertainment_amphitheater_shows();
    int colosseum_shows = city_entertainment_theater_shows();

    int theater_coverage = city_culture_coverage_theater();
    int amphitheater_coverage = city_culture_coverage_amphitheater();
    int colosseum_coverage = city_culture_coverage_theater();

    draw_building_row( 64, BUILDING_THEATER,      34,  500, theater_shows,      theater_coverage);
    draw_building_row( 84, BUILDING_AMPHITHEATER, 36,  800, amphitheater_shows, amphitheater_coverage);
    draw_building_row(104, BUILDING_COLOSSEUM,    38, 1500, colosseum_shows,    colosseum_coverage);
    draw_hippodrome_row(123);

    lang_text_draw_multiline(58, 7 + get_entertainment_advice(), 60, 148, 512, FONT_NORMAL_BLACK);

    draw_festival_info();

    return ADVISOR_HEIGHT;
}

static void draw_foreground(void)
{
    if (!city_festival_is_planned()) {
        button_border_draw(102, 280, 300, 20, focus_button_id == 1);
    }
}

static int handle_mouse(const mouse *m)
{
    return generic_buttons_handle_mouse(m, 0, 0, hold_festival_button, 1, &focus_button_id);
}

static void button_hold_festival(int param1, int param2)
{
    if (!city_festival_is_planned()) {
        window_hold_festival_show();
    }
}

static int get_tooltip_text(void)
{
    if (focus_button_id) {
        return 112;
    } else {
        return 0;
    }
}

const advisor_window_type *window_advisor_entertainment(void)
{
    static const advisor_window_type window = {
        draw_background,
        draw_foreground,
        handle_mouse,
        get_tooltip_text
    };
    focus_button_id = 0;
    return &window;
}
