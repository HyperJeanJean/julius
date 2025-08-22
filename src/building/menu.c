#include "menu.h"

#include "city/buildings.h"
#include "core/config.h"
#include "empire/city.h"
#include "game/tutorial.h"
#include "scenario/building.h"

#define BUILD_MENU_ITEM_MAX 30

static const building_type MENU_BUILDING_TYPE[BUILD_MENU_MAX][BUILD_MENU_ITEM_MAX] = {
    {BUILDING_HOUSE_VACANT_LOT, 0},
    {BUILDING_CLEAR_LAND, 0},
    {BUILDING_ROAD, 0},
    {BUILDING_WELL, BUILDING_FOUNTAIN, BUILDING_DRAGGABLE_RESERVOIR, BUILDING_AQUEDUCT, 0},
    {BUILDING_BATHHOUSE, BUILDING_BARBER, BUILDING_DOCTOR, BUILDING_HOSPITAL, 0},
    {BUILDING_MENU_SMALL_TEMPLES, BUILDING_MENU_LARGE_TEMPLES, BUILDING_ORACLE, 0},
    {BUILDING_SCHOOL, BUILDING_ACADEMY, BUILDING_LIBRARY, BUILDING_MISSION_POST, 0},
    {BUILDING_THEATER, BUILDING_AMPHITHEATER, BUILDING_COLOSSEUM, BUILDING_HIPPODROME,
        BUILDING_ACTOR_COLONY, BUILDING_GLADIATOR_SCHOOL, BUILDING_LION_HOUSE, BUILDING_CHARIOT_MAKER, 0},
    {BUILDING_FORUM, BUILDING_SENATE,
        BUILDING_GOVERNORS_HOUSE, BUILDING_GOVERNORS_VILLA, BUILDING_GOVERNORS_PALACE,
        BUILDING_SMALL_STATUE, BUILDING_MEDIUM_STATUE, BUILDING_LARGE_STATUE, BUILDING_TRIUMPHAL_ARCH, 0},
    {BUILDING_ENGINEERS_POST, BUILDING_GARDENS, BUILDING_PLAZA, BUILDING_LOW_BRIDGE, BUILDING_SHIP_BRIDGE,
        BUILDING_DOCK, BUILDING_SHIPYARD, BUILDING_WHARF, 0},
    {BUILDING_PREFECTURE, BUILDING_WALL, BUILDING_TOWER, BUILDING_GATEHOUSE,
        BUILDING_FORT, BUILDING_MILITARY_ACADEMY, BUILDING_BARRACKS, 0},
    {BUILDING_MENU_FARMS, BUILDING_MENU_RAW_MATERIALS, BUILDING_MENU_WORKSHOPS,
        BUILDING_MARKET, BUILDING_GRANARY, BUILDING_WAREHOUSE, 0},
    {BUILDING_WHEAT_FARM, BUILDING_VEGETABLE_FARM, BUILDING_FRUIT_FARM,
        BUILDING_PIG_FARM, BUILDING_OLIVE_FARM, BUILDING_VINES_FARM, 0},
    {BUILDING_CLAY_PIT, BUILDING_TIMBER_YARD, BUILDING_MARBLE_QUARRY, BUILDING_IRON_MINE, 0},
    {BUILDING_POTTERY_WORKSHOP, BUILDING_FURNITURE_WORKSHOP, BUILDING_OIL_WORKSHOP,
        BUILDING_WINE_WORKSHOP, BUILDING_WEAPONS_WORKSHOP, 0},
    {BUILDING_MENU_SMALL_TEMPLES, BUILDING_SMALL_TEMPLE_CERES, BUILDING_SMALL_TEMPLE_NEPTUNE,
        BUILDING_SMALL_TEMPLE_MERCURY, BUILDING_SMALL_TEMPLE_MARS, BUILDING_SMALL_TEMPLE_VENUS, 0},
    {BUILDING_MENU_LARGE_TEMPLES, BUILDING_LARGE_TEMPLE_CERES, BUILDING_LARGE_TEMPLE_NEPTUNE,
        BUILDING_LARGE_TEMPLE_MERCURY, BUILDING_LARGE_TEMPLE_MARS, BUILDING_LARGE_TEMPLE_VENUS, 0},
    {BUILDING_FORT_LEGIONARIES, BUILDING_FORT_JAVELIN, BUILDING_FORT_MOUNTED, 0},
};
static int menu_enabled[BUILD_MENU_MAX][BUILD_MENU_ITEM_MAX];

static int changed = 1;

void building_menu_enable_all(void)
{
    for (int sub = 0; sub < BUILD_MENU_MAX; sub++) {
        for (int item = 0; item < BUILD_MENU_ITEM_MAX; item++) {
            menu_enabled[sub][item] = 1;
        }
    }
}

static int building_enabled_normal(building_type type)
{
    switch (type) {
        case BUILDING_NONE:
            return 0;

        case BUILDING_WHEAT_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_WHEAT);
        case BUILDING_VEGETABLE_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_VEGETABLES);
        case BUILDING_FRUIT_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_FRUIT);
        case BUILDING_PIG_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_MEAT);
        case BUILDING_OLIVE_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_OLIVES);
        case BUILDING_VINES_FARM:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_VINES);

        case BUILDING_CLAY_PIT:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_CLAY);
        case BUILDING_TIMBER_YARD:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_TIMBER);
        case BUILDING_IRON_MINE:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_IRON);
        case BUILDING_MARBLE_QUARRY:
            return scenario_building_allowed(type) && empire_can_produce_resource(RESOURCE_MARBLE);

        case BUILDING_POTTERY_WORKSHOP:
            return scenario_building_allowed(type) && empire_can_produce_resource_potentially(RESOURCE_POTTERY);
        case BUILDING_FURNITURE_WORKSHOP:
            return scenario_building_allowed(type) && empire_can_produce_resource_potentially(RESOURCE_FURNITURE);
        case BUILDING_OIL_WORKSHOP:
            return scenario_building_allowed(type) && empire_can_produce_resource_potentially(RESOURCE_OIL);
        case BUILDING_WINE_WORKSHOP:
            return scenario_building_allowed(type) && empire_can_produce_resource_potentially(RESOURCE_WINE);
        case BUILDING_WEAPONS_WORKSHOP:
            return scenario_building_allowed(type) && empire_can_produce_resource_potentially(RESOURCE_WEAPONS);

        case BUILDING_TRIUMPHAL_ARCH:
            return city_buildings_triumphal_arch_available();

        default:
            return scenario_building_allowed(type);
    }
}

static int building_enabled_tutorial1_start(building_type type)
{
    switch (type) {
        case BUILDING_HOUSE_VACANT_LOT:
        case BUILDING_CLEAR_LAND:
        case BUILDING_WELL:
        case BUILDING_ROAD:
            return building_enabled_normal(type);
        default:
            return 0;
    }
}

static int building_enabled_tutorial1_after_fire(building_type type)
{
    switch (type) {
        case BUILDING_PREFECTURE:
        case BUILDING_MARKET:
            return building_enabled_normal(type);
        default:
            return building_enabled_tutorial1_start(type);
    }
}

static int building_enabled_tutorial1_after_collapse(building_type type)
{
    switch (type) {
        case BUILDING_ENGINEERS_POST:
        case BUILDING_SENATE:
            return building_enabled_normal(type);
        default:
            return building_enabled_tutorial1_after_fire(type);
    }
}

static int building_enabled_tutorial2_start(building_type type)
{
    switch (type) {
        case BUILDING_HOUSE_VACANT_LOT:
        case BUILDING_CLEAR_LAND:
        case BUILDING_WELL:
        case BUILDING_ROAD:
        case BUILDING_PREFECTURE:
        case BUILDING_ENGINEERS_POST:
        case BUILDING_SENATE:
        case BUILDING_MARKET:
        case BUILDING_GRANARY:
        case BUILDING_MENU_FARMS:
        case BUILDING_WHEAT_FARM:
        case BUILDING_VEGETABLE_FARM:
        case BUILDING_FRUIT_FARM:
        case BUILDING_PIG_FARM:
        case BUILDING_OLIVE_FARM:
        case BUILDING_VINES_FARM:
        case BUILDING_MENU_SMALL_TEMPLES:
        case BUILDING_SMALL_TEMPLE_CERES:
        case BUILDING_SMALL_TEMPLE_NEPTUNE:
        case BUILDING_SMALL_TEMPLE_MERCURY:
        case BUILDING_SMALL_TEMPLE_MARS:
        case BUILDING_SMALL_TEMPLE_VENUS:
            return building_enabled_normal(type);
        default:
            return 0;
    }
}

static int building_enabled_tutorial2_up_to_250(building_type type)
{
    switch (type) {
        case BUILDING_DRAGGABLE_RESERVOIR:
        case BUILDING_AQUEDUCT:
        case BUILDING_FOUNTAIN:
            return building_enabled_normal(type);
        default:
            return building_enabled_tutorial2_start(type);
    }
}

static int building_enabled_tutorial2_up_to_450(building_type type)
{
    switch (type) {
        case BUILDING_GARDENS:
        case BUILDING_ACTOR_COLONY:
        case BUILDING_THEATER:
        case BUILDING_BATHHOUSE:
        case BUILDING_SCHOOL:
            return building_enabled_normal(type);
        default:
            return building_enabled_tutorial2_up_to_250(type);
    }
}

static int building_enabled_tutorial2_after_450(building_type type)
{
    switch (type) {
        case BUILDING_MENU_RAW_MATERIALS:
        case BUILDING_CLAY_PIT:
        case BUILDING_TIMBER_YARD:
        case BUILDING_IRON_MINE:
        case BUILDING_MARBLE_QUARRY:
        case BUILDING_MENU_WORKSHOPS:
        case BUILDING_POTTERY_WORKSHOP:
        case BUILDING_FURNITURE_WORKSHOP:
        case BUILDING_OIL_WORKSHOP:
        case BUILDING_WINE_WORKSHOP:
        case BUILDING_WEAPONS_WORKSHOP:
        case BUILDING_WAREHOUSE:
        case BUILDING_FORUM:
        case BUILDING_AMPHITHEATER:
        case BUILDING_GLADIATOR_SCHOOL:
            return building_enabled_normal(type);
        default:
            return building_enabled_tutorial2_up_to_450(type);
    }
}

void building_menu_update(void)
{
    tutorial_build_buttons tutorial_buttons = tutorial_get_build_buttons();
    for (int sub = 0; sub < BUILD_MENU_MAX; sub++) {
        for (int item = 0; item < BUILD_MENU_ITEM_MAX; item++) {
            int building_type = MENU_BUILDING_TYPE[sub][item];
            int *menu_item = &menu_enabled[sub][item];
            switch (tutorial_buttons) {
                case TUT1_BUILD_START:
                    *menu_item = building_enabled_tutorial1_start(building_type);
                    break;
                case TUT1_BUILD_AFTER_FIRE:
                    *menu_item = building_enabled_tutorial1_after_fire(building_type);
                    break;
                case TUT1_BUILD_AFTER_COLLAPSE:
                    *menu_item = building_enabled_tutorial1_after_collapse(building_type);
                    break;
                case TUT2_BUILD_START:
                    *menu_item = building_enabled_tutorial2_start(building_type);
                    break;
                case TUT2_BUILD_UP_TO_250:
                    *menu_item = building_enabled_tutorial2_up_to_250(building_type);
                    break;
                case TUT2_BUILD_UP_TO_450:
                    *menu_item = building_enabled_tutorial2_up_to_450(building_type);
                    break;
                case TUT2_BUILD_AFTER_450:
                    *menu_item = building_enabled_tutorial2_after_450(building_type);
                    break;
                default:
                    *menu_item = building_enabled_normal(building_type);
                    break;
            }
        }
    }
    if (!config_get(CONFIG_UI_ALLOW_CYCLING_TEMPLES)) {
        menu_enabled[BUILD_MENU_SMALL_TEMPLES][0] = 0;
        menu_enabled[BUILD_MENU_LARGE_TEMPLES][0] = 0;
    }

    changed = 1;
}

int building_menu_count_items(int submenu)
{
    int count = 0;
    for (int item = 0; item < BUILD_MENU_ITEM_MAX; item++) {
        if (menu_enabled[submenu][item] && MENU_BUILDING_TYPE[submenu][item] > 0) {
            count++;
        }
    }
    return count;
}

int building_menu_next_index(int submenu, int current_index)
{
    for (int i = current_index + 1; i < BUILD_MENU_ITEM_MAX; i++) {
        if (MENU_BUILDING_TYPE[submenu][i] <= 0) {
            return 0;
        }
        if (menu_enabled[submenu][i]) {
            return i;
        }
    }
    return 0;
}

building_type building_menu_type(int submenu, int item)
{
    return MENU_BUILDING_TYPE[submenu][item];
}

build_menu_group building_menu_for_type(building_type type)
{
    for (int sub = 0; sub < BUILD_MENU_MAX; sub++) {
        for (int item = 0; item < BUILD_MENU_ITEM_MAX && MENU_BUILDING_TYPE[sub][item]; item++) {
            if (MENU_BUILDING_TYPE[sub][item] == type) {
                return sub;
            }
        }
    }
    return -1;
}

int building_menu_is_enabled(building_type type)
{
    for (int sub = 0; sub < BUILD_MENU_MAX; sub++) {
        for (int item = 0; item < BUILD_MENU_ITEM_MAX && MENU_BUILDING_TYPE[sub][item]; item++) {
            if (MENU_BUILDING_TYPE[sub][item] == type) {
                return menu_enabled[sub][item];
            }
        }
    }
    return 0;
}

int building_menu_has_changed(void)
{
    if (changed) {
        changed = 0;
        return 1;
    }
    return 0;
}
