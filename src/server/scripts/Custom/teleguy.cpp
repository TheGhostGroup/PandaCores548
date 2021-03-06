#include "ScriptPCH.h"

struct Location
{
    uint32 mapId;
    float x;
    float y;
    float z;
    float o;
};

struct TeleData
{
    uint32 action;
    uint32 subAction;
    std::string name;
    Location loc;
    uint32 team;
    bool isEmpty;
    bool isGrub;
};

#define LOCATIONS_COUNT 20

TeleData data[] =
{
    {1251, 0, "Duel Zone.",             {1, 5465.16f, -3724.59f, 1593.44f, 0.0f},       ALLIANCE, false, false},
    {1203, 0, "Darnassus.",             {1, 9947.52f, 2482.73f, 1316.21f, 0.0f},        ALLIANCE, false, false},
    {1216, 0, "Exodar.",                {530, -3965.69f, -11653.59f, -138.84f, 0.0f},   ALLIANCE, false, false},
    {1206, 0, "Stormwind.",             {0, -8960.14f, 516.266f, 96.3568f, 0.0f},       ALLIANCE, false, false},
    {1224, 0, "Ironforge.",             {0, -4924.07f, -951.95f, 501.55f, 5.40f},       ALLIANCE, false, false},
    {1226, 0, "Shrine of Seven Stars.", {870, 881.49f, 308.12f, 503.12f, 0.0f},         ALLIANCE, false, false},

    {1251, 0, "Duel Zone.",             {870, -229.73f, -5376.26f, 123.07f, 0.0f},      HORDE,    false, false},
    {1215, 0, "Orgrimmar.",             {1, 1424.53f, -4411.07f, 73.91f, 0.0f},         HORDE,    false, false},
    {1217, 0, "Silvermoon.",            {530, 9338.74f, -7277.27f, 13.7895f, 0.0f},     HORDE,    false, false},
    {1213, 0, "Undercity.",             {0, 1819.71f, 238.79f, 60.5321f, 0.0f},         HORDE,    false, false},
    {1225, 0, "Thunder Bluff.",         {1, -1273.88f, 81.62f, 128.42f, 0.0f},          HORDE,    false, false},
    {1227, 0, "Shrine of Two Moons.",   {870, 1570.21f, 906.84f, 474.23f, 0.0f},        HORDE,    false, false},

    {1287, 0, "Shattrath City.",        {530, -1850.2f, 5435.8f, -10.9f, 0.0f},         0,        false, false},
    {1205, 0, "Dalaran.",               {571, 5804.14f, 624.770f, 647.7670f, 1.64f},    0,        false, false},
    {1207, 0, "Nagrand.",               {530, -2504.31f, 6445.08f, 200.43f, 1.64f},     0,        false, false},

    {5550, 0, "[Outdoor PvP] ->",       {0, 0, 0, 0, 0},                                0,        true,  false},
    {1248, 5550, "Roing of Trials.",    {530, -2049.26f, 6662.82f, 13.06f, 0.0f},       0,        false, false},
    {1249, 5550, "Circle of Blood.",    {530, 2839.43f, 5930.16f, 11.20f, 0.0f},        0,        false, false},
    {1250, 5550, "The mauls.",          {1, -3761.24f, 1131.89f, 132.96f, 0.0f},        0,        false, false},
    {1252, 5550, "Gurubashi arena.",    {0, -13312.44f, 61.878f,  22.193f, 0.0f},       0,        false, false},
};

class npc_teleguy : public CreatureScript
{

public:
    npc_teleguy() : CreatureScript("npc_teleguy") {}
    
    void AddAction(Player *player, uint16 index)
    {
        player->ADD_GOSSIP_ITEM( 5, data[index].name.c_str(), GOSSIP_SENDER_MAIN, data[index].action);
    }

    bool OnGossipHello(Player* player, Creature* _Creature)
    {
        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];
            if (_data.subAction == 0 && (_data.team == player->GetTeam() || !_data.team))
                AddAction(player, i);
        }
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
        return true;
    }
    
    bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        int16 actionIndex = -1;
        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];
            if (_data.action == action)
                actionIndex = i;
        }

        if (actionIndex < 0)
            return false;

        if (data[actionIndex].isEmpty)
        {
            SendSubMenu(player, _Creature, action);
            return true;
        }

        if(!player->getAttackers().empty())
        {
            player->CLOSE_GOSSIP_MENU();
            _Creature->MonsterSay("You are in combat!", LANG_UNIVERSAL, NULL);
            return false;
        }

        player->CLOSE_GOSSIP_MENU();

        TelePlayerByAction(player, actionIndex);

        return true;
    }
    
    void TelePlayerByAction(Player *player, uint16 actionIndex)
    {
        Location loc = data[actionIndex].loc;
        player->TeleportTo(loc.mapId, loc.x, loc.y, loc.z, loc.z);
    }

    void SendSubMenu(Player *player, Creature *creature, uint16 submenu)
    {
        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];
            if (_data.subAction == submenu && (_data.team == player->GetTeam() || !_data.team))
                AddAction(player, i);
        }
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
};

struct TeleInfo
{
	std::string text;
	WorldLocation loc;
};

std::map<uint32, std::vector<TeleInfo>> teleMenu
{
    // Разное:
    { 6,{ { "Арена в Храме Белого Тигра",{ 870, 3599.708252f, 543.187927f, 615.807190f } },
        { "Арена в Сумеречном нагорье",{ 0, -4208.134766f, -5112.048828f, 25.845881f } },
        { "Арена в Зул'Драке",{ 571, 5793.524902f, -3020.279785f, 286.290100f } },
        { "Арена в Награнде",{ 530, -723.467346f, 7932.981445f, 58.751961f } },
        { "Арена в Танарисе",{ 1, -7113.511719f, -3793.874756f, 8.410527f } },
        { "Арена Гурубаши",{ 0, -13226.339844f, 231.952881f, 33.284954f } },
        { "Арена в Круге Крови",{ 530, 2838.452393f, 5927.564941f, 11.070838f } },
        { "Арена в Награнде",{ 530, -2045.597168f, 6653.827148f, 13.054525f } },
        { "Остров Новолуния",{ 974, -3980.063477f, 6231.134277f, 14.134694f } } } },
    //  Пандария:
    { 5,{ { "Осада Оргриммара (ОО)",{ 870, 1198.294434f, 648.816833f, 347.513977f } },
        { "Подземелья Могу'шан (ПМШ)",{ 870, 3984.708252f, 1107.497192f, 497.145966f } },
        { "Престол Гроз (ПГ)",{ 1064, 7241.394043f, 5038.210938f, 76.220665f } },
        { "Сердце Страха (СС)",{ 870, 170.666321f, 4041.114990f, 255.914886f } },
        { "Терраса Вечной Весны (ТЧВ)",{ 870, 953.370483f, -58.449921f, 510.107422f } },
        { "Врата Заходящего Солнца",{ 870, 682.647339f, 2079.921143f, 371.710358f } },
        { "Дворец Могу'шан",{ 870, 1390.274536f, 439.529663f, 479.034119f } },
        { "Хмелеварня Буйных портеров",{ 870, -691.994080f, 1269.524292f, 136.084671f } },
        { "Монастырь Шадо-Пан",{ 870, 3627.857422f, 2536.668213f, 769.950073f } },
        { "Осада Храма Нюцзао",{ 870, 1419.895508f, 5082.514160f, 126.282410f } },
        { "Храм Нефритовой Змеи",{ 870, 961.467407f, -2463.143066f, 180.581985f } },
        { "Таверна \"В туманах\"",{ 870, 812.400696f, -178.757324f, 415.131195f } },
        { "Деревня Рыболовов",{ 870, -1479.061157f, -231.548401f, 5.112297f } },
        { "Вневременный остров",{ 870, -650.147034f, -4899.938477f, 2.056325f } },
        { "Остров Великанов",{ 870, 5751.615723f, 1162.714722f, 1.559572f } },
        { "Остров Грома",{ 1064, 6800.775879f, 5478.339844f, 29.928785f } }} },

    // Катаклизм:
    { 4,{ { "Душа Дракона (ДД)",{ 1, -8273.000977f, -4515.538574f, -219.056763f } },
        { "Крепость Барадин (КБ)",{ 732, -1204.470093f, 1081.330078f, 120.354973f } },
        { "Огненные Простороры (ОП)",{ 1, 3990.985352f, -2951.045654f, 1002.547424f } },
        { "Сумеречный Бастион (СБ)",{ 0, -4888.555176f, -4242.727539f, 827.762878f } },
        { "Твердыня Крыла Тьмы (ТКТ)",{ 0, -7538.248047f, -1198.454346f, 477.720581f } },
        { "Трон Четырёх Ветров (ТЧВ)",{ 1, -11355.091797f, 57.796021f, 723.884033f } },
        { "Вершина Смерча",{ 1, -11512.080078f, -2309.046143f, 608.393677f } },
        { "Время Сумерек",{ 1, -8292.992188f, -4574.514648f, -225.006012f } },
        { "Грим Батол",{ 0, -4063.872559f, -3451.425537f, 280.219177f } },
        { "Затерянный город Тол'вир",{ 1, -10672.040039f, -1304.768555f, 15.353432f } },
        { "Зул'Аман",{ 530, 6851.040527f, -7994.026855f, 192.370026f } },
        { "Зул'Гуруб",{ 0, -11916.274414f, -1209.298706f, 92.288185f } },
        { "Источник Вечности",{ 1, -8598.936523f, -4013.136719f, -205.547363f } },
        { "Каменные Недра",{ 646, 1023.826843f, 641.982727f, 156.672241f } },
        { "Конец Времен",{ 1, -8300.198242f, -4460.448242f, -208.361374f } },
        { "Крепость Темного Клыка",{ 0, -245.108505f, 1535.278564f, 76.884575f } },
        { "Мертвые Копи",{ 0, -11208.404297f, 1673.227905f, 24.640856f } },
        { "Пещеры Черной Горы",{ 0, -7571.501953f, -1323.749756f, 245.536789f } },
        { "Трон Приливов",{ 0, -5597.484863f, 5410.770020f, -1798.624512f } },
        { "Чертоги Созидания",{ 1, -10183.499023f, -2005.540039f, 54.850933f } },
        { "Огненная Передовая",{ 861, 1026.512939f, 326.450012f, 43.128017f } },
        { "Трон Теразан",{ 646, 2356.175537f, 168.990829f, 181.397308f } } } },

    // Нордскол:
    { 3,{ { "Цитадель Ледяной Короны (ЦЛК)",{ 571, 5798.973145f, 2075.413330f, 636.064270f } },
        { "Ульдуар",{ 571, 9329.967773f, -1117.265991f, 1245.147095f } },
        { "Наксрамас",{ 571, 3681.921387f, -1269.339722f, 243.550156f } },
        { "Испытание Крестоносца (ИК)",{ 571, 8515.189453f, 730.540405f, 558.248169f } },
        { "Рубиновое Святилище (РС)",{ 571, 3599.100342f, 198.728455f, -113.918884f } },
        { "Логово Ониксии",{ 1, -4706.007324f, -3725.238037f, 54.216221f } },
        { "Азжол-Неруб",{ 571, 3677.481445f, 2166.978760f, 35.823750f } },
        { "Вершина Утгард",{ 571, 1242.485474f, -4857.526367f, 217.807739f } },
        { "Гундрак",{ 571, 6937.031250f, -4436.122070f, 450.518768f } },
        { "Испытание Чемпиона",{ 571, 8575.139648f, 792.034912f, 558.514832f } },
        { "Крепость Драк'Тарон",{ 571, 4782.254395f, -2045.779663f, 229.431503f } },
        { "Крепость Утгард",{ 571, 1224.328735f, -4863.303223f, 41.248013f } },
        { "Кузня Душ",{ 571, 5663.309570f, 2012.766235f, 798.042419f } },
        { "Нексус",{ 571, 3891.989014f, 6985.246094f, 69.488075f } },
        { "Очищение Стратхольма",{ 1, -8755.321289f, -4450.678223f, -199.746170f } },
        { "Чертоги Молний",{ 571, 9183.208984f, -1384.422485f, 1110.215332f } },
        { "Яма Сарона",{ 571, 5600.267578f, 2017.103882f, 798.042358f } },
        { "Лес Хрустальной Песни",{ 571, 5454.924805f, 79.064369f, 176.894958f } },
        { "Дун Ниффелем",{ 571, 7348.333008f, -2605.087891f, 814.849304f } },
        { "Лагерь Д.Э.Г.О.Ж.",{ 571, 3228.040527f, 5279.141602f, 46.991146f } },
        { "Озеро Ледяных Оков (ОЛО)",{ 571, 4603.230469f, 2846.640381f, 396.896454f } },
        { "Лагерь Хротгара",{ 571, 9920.071289f, 1139.714355f, 58.439980f } } } },

    // Запределье:
    { 2,{ {"Черный Храм (ЧХ)",{ 530, -3642.185791f, 315.732849f, 35.080578f } },
        { "Плато Солнечного Колодца (ПСК)",{ 530, 12560.092773f, -6774.750488f, 15.031774f } },
        { "Логово Магтеридона",{ 530, -314.911316f, 3089.671875f, -116.477043f } },
        { "Змеиное Святилище",{ 530, 795.544312f, 6864.642090f, -64.882057f } },
        { "Крепость Бурь (КБ)",{ 530, 3088.303467f, 1383.490479f, 184.822083f } },
        { "Каражан",{ 0, -11119.043945f, -2010.794067f, 47.081917f } },
        { "Логово Груула",{ 530, 3538.100586f, 5095.592773f, 3.827646f } },
        { "Битва за Гору Хиджал",{ 1, -8174.143555f, -4176.042969f, -166.160446f } },
        { "Аркатрац",{ 530, 3308.318115f, 1341.447876f, 505.559998f } },
        { "Аукенайские Гробницы",{ 530, -3361.969238f, 5218.605957f, -101.048927f } },
        { "Бастионы Адского Пламени",{ 530, -362.368958f, 3075.630127f, -15.044868f } },
        { "Ботаника",{ 530, 3404.688721f, 1490.116821f, 182.837402f } },
        { "Гробницы Маны",{ 530, -3083.988770f, 4942.971680f, -101.047379f } },
        { "Кузня Крови",{ 530, -300.034454f, 3159.795898f, 31.686928f } },
        { "Механар",{ 530, 2872.008789f, 1554.671753f, 252.159927f } },
        { "Нижетопь",{ 530, 781.553528f, 6757.112793f, -72.538170f } },
        { "Паровое Подземелье",{ 530, 817.702881f, 6936.059570f, -80.626816f } },
        { "Разрушенные Залы",{ 530, -309.308777f, 3076.211670f, -3.646897f } },
        { "Сетеккские Залы",{ 530, -3361.872803f, 4665.686035f, -101.046959f } },
        { "Старые прдгорья Хилсбрада",{ 1, -8364.833984f, -4058.000977f, -208.264832f } },
        { "Темный Лабиринт",{ 530, -3635.138672f, 4943.125000f, -101.049004f } },
        { "Терраса Магистров",{ 530, 12885.387695f, -7334.700195f, 65.487663f } },
        { "Узилище",{ 530, 724.600220f, 7011.446777f, -72.262093f } },
        { "Черные Топи",{ 1, -8751.717773f, -4195.399414f, -209.498672f } },
        { "Лагерь Драконьей Пасти",{ 530, -5109.739258f, 603.126282f, 85.105316f } },
        { "Приют Уэйна",{ 530, -2389.543213f, 2902.149414f, -55.578075f } },
        { "Лагерь Черного Ветра",{ 530, -3365.530762f, 3649.803467f, 284.591309f } },
        { "Халаа",{ 530, -1571.396729f, 7957.963867f, -22.601425f } },
        { "Спореггар",{ 530, 243.163574f, 8505.202148f, 22.840126f } },
        { "кенарийский Оплот",{ 530, -189.839386f, 5518.477539f, 28.825733f } },
        { "Огри'ла",{ 530, 2323.647949f, 7282.838379f, 365.618591f } },
        { "Деревня Кирин'вар",{ 530, 2264.510742f, 2271.068848f, 97.540649f } },
        { "Трон Кил'джедена",{ 530, 909.243591f, 2269.343994f, 310.259460f } } } },

    // Калимдор и Восточные Королевства :
        { 1,{ { "Огненные Недра (ОН)",{ 0, -7511.410645f, -1042.036011f, 180.911377f } },
            { "Логово Крыла Тьмы (ЛКТ)",{ 0, -7659.681641f, -1220.593994f, 287.789124f } },
            { "Руины Ан'Киража",{ 1, -8415.337891f, 1505.528198f, 31.278450f } },
            { "Храм Ан'Киража",{ 1, -8237.951172f, 1990.962036f, 129.072693f } },
            { "Глубины Черной Горы",{ 0, -7179.401367f, -922.581665f, 166.169739f } },
            { "Гномреган",{ 0, -5143.506348f, 895.267761f, 287.394073f } },
            { "Забытый Город",{ 1, -3789.423340f, 1242.360352f, 160.269928f } },
            { "Залы Алого Ордена",{ 0, 2872.347168f, -819.494568f, 160.332001f } },
            { "Зул'Фаррак",{ 1, -6796.538086f, -2890.864746f, 8.881607f } },
            { "Крепость Темного Клыка",{ 0, -245.108505f, 1535.278564f, 76.884575f } },
            { "Курганы Иглошкурых",{ 1, -4658.570312f, -2525.889648f, 81.444489f } },
            { "Лабиринты Иглошкурых",{ 1, -4466.629883f, -1669.908203f, 81.892502f } },
            { "Мародон",{ 1, -1187.513550f, 2879.690430f, 85.815750f } },
            { "Мертвые Копи",{ 0, -11208.404297f, 1673.227905f, 24.640856f } },
            { "Монастырь Алого Ордена",{ 0, 2913.326416f, -802.447327f, 160.332001f } },
            { "Некроситет",{ 0, 1257.583252f, -2584.631348f, 92.887039f } },
            { "Непроглядная Пучина",{ 1, 4248.132812f, 743.609131f, -24.830606f } },
            { "Нижняя часть Пика Черной Горы",{ 0, -7526.470703f, -1226.190918f, 285.732758f } },
            { "Огненная Пропасть",{ 1, 1813.999023f, -4416.091309f, -18.220598f } },
            { "Пещеры Стенаний",{ 1, -741.325806f, -2215.993164f, 15.928796f } },
            { "Стратхольм",{ 0, 3356.535400f, -3379.539062f, 144.782700f } },
            { "Стратхольм (черный вход)",{ 0, 3177.213379f, -4038.888916f, 105.441299f } },
            { "Тюрьма",{ 0, -8786.644531f, 829.226624f, 97.650917f } },
            { "Ульдаман",{ 0, -6070.387207f, -2955.664795f, 209.780655f } },
            { "Храм Атал'Хаккара",{ 0, -10291.303711f, -3985.823730f, -70.735390f } },
            { "Подземный Поезд",{ 369, -19.843245f, 37.441551f, -4.297364f } },
            { "Пещеры Времени",{ 1, -8445.805664f, -4362.279297f, -176.302505f } },
            { "Даларанский кратер",{ 0, 310.012177f, 294.056641f, 80.379440f } },
            { "Поместье Черного Ворона",{ 0, 23.520702f, -1603.598022f, 195.420502f } },
            { "Талассийский перевал",{ 0, 3465.497314f, -4476.250000f, 137.350983f } },
            { "Лунная Поляна",{ 1, 7792.039062f, -2405.552490f, 489.165863f } } }  },
};

class npc_teleoprter : public CreatureScript
{
public:
	npc_teleoprter() : CreatureScript("npc_teleoprter") { }

    uint32 PriceInGold = 5000 *10000; // 5000 golds

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
        pPlayer->ADD_GOSSIP_ITEM(5, "Калимдор и Восточные Королевства", GOSSIP_SENDER_SEC_STABLEMASTER, 1);
        pPlayer->ADD_GOSSIP_ITEM(5, "Запределье", GOSSIP_SENDER_SEC_STABLEMASTER, 2);
        pPlayer->ADD_GOSSIP_ITEM(5, "Нордскол", GOSSIP_SENDER_SEC_STABLEMASTER, 3);
        pPlayer->ADD_GOSSIP_ITEM(5, "Катаклизм", GOSSIP_SENDER_SEC_STABLEMASTER, 4);
        pPlayer->ADD_GOSSIP_ITEM(5, "Пандария", GOSSIP_SENDER_SEC_STABLEMASTER, 5);
        pPlayer->ADD_GOSSIP_ITEM(5, "Разное", GOSSIP_SENDER_SEC_STABLEMASTER, 6);
		pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
        player->PlayerTalkClass->SendCloseGossip();
        if (sender == GOSSIP_SENDER_SEC_STABLEMASTER)
        {
            for (uint32 i =0; i < teleMenu[action].size(); ++i)
            {
                auto inf = teleMenu[action][i];
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, inf.text.c_str(), action, i+1, "", PriceInGold, false);
            }
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }
		
        if (!player->HasEnoughMoney(uint64(PriceInGold)))
            player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, NULL, 0, 0);
        else if (teleMenu[sender].size() > (action - 1))
        {
            player->TeleportTo(teleMenu[sender][action - 1].loc);
            player->ModifyMoney(-1*PriceInGold);
        }
        player->CLOSE_GOSSIP_MENU();
        return true;
	}
};

void AddSC_npc_teleguy()
{
    new npc_teleguy();
    new npc_teleoprter();
}
