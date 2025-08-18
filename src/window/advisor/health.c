#include "health.h"
#include "building/count.h"
#include "city/culture.h"
#include "city/health.h"
#include "city/houses.h"
#include "city/population.h"
#include "core/string.h"
#include "graphics/image.h"
#include "graphics/lang_text.h"
#include "graphics/panel.h"
#include "graphics/text.h"
#include "scenario/building.h"

#define ADVISOR_HEIGHT 18

static int get_health_advice(void)
{
    house_demands *demands = city_houses_demands();
    switch (demands->health) {
        case 1:
            return demands->requiring.bathhouse ? 1 : 0;
        case 2:
            return demands->requiring.barber ? 3 : 2;
        case 3:
            return demands->requiring.clinic ? 5 : 4;
        case 4:
            return 6;
        default:
            return 7;
    }
}

static void draw_disabled_building_row(int y_offset, int building_text_id)
{
    const uint8_t *hyphen = string_from_ascii("-");
    text_draw(hyphen, 46, y_offset, FONT_NORMAL_WHITE, 0);
    lang_text_draw(8, building_text_id, 67, y_offset, FONT_NORMAL_WHITE);
    text_draw_centered(hyphen, 150, y_offset, 100, FONT_NORMAL_WHITE, 0);
    text_draw_centered(hyphen, 290, y_offset, 120, FONT_NORMAL_WHITE, 0);
    text_draw_centered(hyphen, 440, y_offset, 160, FONT_NORMAL_WHITE, 0);
}

static void draw_building_row(int y_offset, building_type building, int building_text_id)
{
    if (!scenario_building_allowed(building) && building_count_total(building) <= 0) {
        draw_disabled_building_row(y_offset, building_text_id + 1);
        return;
    }

    lang_text_draw_amount(8, building_text_id, building_count_total(building), 40, y_offset, FONT_NORMAL_WHITE);
    text_draw_number_centered(building_count_active(building), 150, y_offset, 100, FONT_NORMAL_WHITE);
    lang_text_draw_centered(56, 2, 290, y_offset, 120, FONT_NORMAL_WHITE);
    lang_text_draw_centered(56, 2, 440, y_offset, 160, FONT_NORMAL_WHITE);
}

static void draw_hospital_row(int y_offset)
{
    if (!scenario_building_allowed(BUILDING_HOSPITAL) && building_count_total(BUILDING_HOSPITAL) <= 0) {
        draw_disabled_building_row(y_offset, 31);
        return;
    }

    lang_text_draw_amount(8, 30, building_count_total(BUILDING_HOSPITAL), 40, y_offset, FONT_NORMAL_WHITE);
    text_draw_number_centered(building_count_active(BUILDING_HOSPITAL), 150, y_offset, 100, FONT_NORMAL_WHITE);

    int width = text_draw_number(1000 * building_count_active(BUILDING_HOSPITAL), '@', " ",
        280, y_offset, FONT_NORMAL_WHITE);
    lang_text_draw(56, 6, 280 + width, y_offset, FONT_NORMAL_WHITE);

    int pct_hospital = city_culture_coverage_hospital();
    if (pct_hospital == 0) {
        lang_text_draw_centered(57, 10, 440, y_offset, 160, FONT_NORMAL_WHITE);
    } else if (pct_hospital < 100) {
        lang_text_draw_centered(57, pct_hospital / 10 + 11, 440, y_offset, 160, FONT_NORMAL_WHITE);
    } else {
        lang_text_draw_centered(57, 21, 440, y_offset, 160, FONT_NORMAL_WHITE);
    }
}

static int draw_background(void)
{
    outer_panel_draw(0, 0, 40, ADVISOR_HEIGHT);
    image_draw(image_group(GROUP_ADVISOR_ICONS) + 6, 10, 10);

    lang_text_draw(56, 0, 60, 12, FONT_LARGE_BLACK);
    if (city_population() >= 200) {
        lang_text_draw_multiline(56, city_health() / 10 + 16, 60, 46, 512, FONT_NORMAL_BLACK);
    } else {
        lang_text_draw_multiline(56, 15, 60, 46, 512, FONT_NORMAL_BLACK);
    }
    lang_text_draw(56, 3, 180, 94, FONT_SMALL_PLAIN);
    lang_text_draw(56, 4, 290, 94, FONT_SMALL_PLAIN);
    lang_text_draw_centered(56, 5, 440, 94, 160, FONT_SMALL_PLAIN);

    inner_panel_draw(32, 108, 36, 5);

    draw_building_row(112, BUILDING_BATHHOUSE, 24);
    draw_building_row(132, BUILDING_BARBER,    26);
    draw_building_row(152, BUILDING_DOCTOR,    28);
    draw_hospital_row(172);

    lang_text_draw_multiline(56, 7 + get_health_advice(), 60, 194, 512, FONT_NORMAL_BLACK);

    return ADVISOR_HEIGHT;
}

const advisor_window_type *window_advisor_health(void)
{
    static const advisor_window_type window = {
        draw_background,
        0,
        0,
        0
    };
    return &window;
}
