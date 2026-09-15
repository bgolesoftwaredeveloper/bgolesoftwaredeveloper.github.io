/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Blowfish symmetric-key block cipher.
 * @details     :   This file contains the core logic for the Blowfish algorithm, 
 * including subkey generation (initialization), the Feistel 
 * function, and block-level encryption and decryption.
 * @version     :   1.0
 * @date        :   2026-05-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/cryptography/blowfish.h"

/**
 * @brief Initial P-array (Permutation) values
 * derived from the fractional digits of PI.
 */
const uint32_t initial_permuation_values[18] = 
{
    0x243F6A88, 0x85A308D3, 0x13198A2E, 0x03707344, 0xA4093822, 0x299F31D0,
    0x082EFA98, 0xEC4E6C89, 0x452821E6, 0x38D01377, 0xBE5466CF, 0x34E90C6C,
    0xC0AC29B7, 0xC97C50DD, 0x3F84D5B5, 0xB5470917, 0x9216D5D9, 0x8979FB1B
};

/**
 * @brief Initial S-boxes (Substitution) values
 * Four boxes, each with 256 32-bit entries, also derived from PI.
 */
const uint32_t initial_substitution_values[4][256] =
{
    {
        0xD1310BA6, 0x98DFB5AC, 0x2FFD72DB, 0xD01ADFB7, 0xB8E1AFED, 0x6A267E96, 0xBA7C9045, 0xF12C7F99,
        0x24A19947, 0xB39115F7, 0x4A6B4942, 0x79D15ED9, 0x297F2D43, 0x2D175254, 0x393A2D73, 0x3B724C21,
        0x3C519751, 0x38561169, 0x170ECED5, 0x717C33E8, 0x3CE36905, 0x12638417, 0xB78E911E, 0x39783577,
        0x78AF7A18, 0xC7C32B29, 0x7EF5440B, 0x384B4056, 0x5E459610, 0x12702127, 0x51E283E6, 0x33800A88,
        0xB1600021, 0x48823C52, 0x88944474, 0x89047974, 0x6739C910, 0x66930444, 0x303D084F, 0xA26F526F,
        0x20556271, 0x011E6348, 0x107481F4, 0x40ABE05D, 0x34080AC0, 0xD346A6CD, 0x2442490A, 0x06B3E991,
        0x9099302F, 0xD7489247, 0x546413E0, 0x106137D1, 0x1A28A941, 0x47AD46F2, 0x34633759, 0x2D7860B3,
        0x301D3960, 0x2724F9A8, 0x723D6983, 0x04C5A154, 0x3B6C242A, 0x3E806D60, 0x64AD210E, 0x5A0302E4,
        0xA3641793, 0x1E01705D, 0x4A009C15, 0xB4566F02, 0x00F8E33B, 0x4896797D, 0x0735E28B, 0x2035F121,
        0x037418F2, 0x323B5F6F, 0x732971EF, 0x060E6F25, 0xAD1CCEB6, 0x6A911145, 0x7620AF01, 0xF94B65B5,
        0x01786968, 0x50319522, 0x789A1201, 0x05E591CB, 0x330B5829, 0x238BD1EB, 0x66B79D7D, 0x79402507,
        0xB2AA7752, 0x5B5B0399, 0x9330D056, 0xA116F343, 0x8C117562, 0x61661C02, 0x8849E937, 0x4062534C,
        0x3841D6CC, 0x5E8AD651, 0xC263AC11, 0x002022C8, 0x6EB879F4, 0x8116900D, 0x065C1717, 0x97782A02,
        0x96C75988, 0x82DC38E9, 0x310D701A, 0x863A293E, 0x32A2F0F7, 0x819ED0F1, 0x1B192770, 0x05C7A684,
        0x83B329B3, 0x5B6C0752, 0x08C8AD13, 0x30BE18CD, 0x99534E27, 0x7A4C626F, 0x82A13D07, 0x60017BED,
        0x105745E6, 0xD2968393, 0x310D6922, 0xBB813904, 0x02773539, 0xA26F92EE, 0xBE403E55, 0x833E01EE,
        0x70529C91, 0x86A07890, 0x5466A901, 0xB95E67C9, 0x58256094, 0x651C818C, 0xA2B12384, 0xA0EF0C30,
        0x04F5E68C, 0x6F54B0B9, 0x4A6A6400, 0x45E56CB0, 0xF94F890D, 0x93884D72, 0xE9B7C75C, 0xDDB17E3F,
        0x05786E45, 0x5897BAF2, 0x7A494459, 0xE6F87A6D, 0x58E2614E, 0xC3F13D1A, 0x4A00C4EF, 0x971484C2,
        0x56E13393, 0x8B75F386, 0x58356614, 0xD56F8403, 0x4ED75D72, 0x138A778A, 0x73C9B61E, 0x0D04A7B7,
        0xB38499D1, 0xC00E19B8, 0x972A0D32, 0xEE7D5A47, 0x5CCF34F2, 0x8B316C4D, 0x305A0A9C, 0x1E596E7E,
        0xD50800A8, 0xB4D984EF, 0x0B80050A, 0x1B18797D, 0x98A536F2, 0xAB1D07A1, 0x7262F6A1, 0x5699478D,
        0x367ED5A3, 0x3D7F4638, 0x0A364491, 0xA991D592, 0x410C5D20, 0x5C9F1A52, 0xF9B19E53, 0x0286F6B6,
        0x787A5B18, 0xD375B822, 0x39E367EB, 0x705A18C6, 0x3300D7D0, 0x2A6BE0F1, 0x76B690A8, 0x49B3C83D,
        0xB177D0A2, 0x5A0C22B4, 0x6424D007, 0x960D8412, 0x1AD8C3D2, 0x83A89745, 0xA4F7540D, 0x1E0942A9,
        0x63343997, 0x060E6627, 0x165B1746, 0x9E7D0DC9, 0x34293D9B, 0x0EBB3340, 0x86D49191, 0xB63750FA,
        0x673B8806, 0xA87D70A3, 0x6543D751, 0xD29688FE, 0xC4C7E951, 0x7D365664, 0xEB022031, 0x2649B2A9,
        0x6968B99A, 0x32A55153, 0x0374164F, 0x64E75062, 0xB571B394, 0xD61B7253, 0x66B0D6F1, 0x33446820,
        0x3081F083, 0xBBED31A2, 0xC6A24021, 0x7030896E, 0xBE271616, 0xB6541D44, 0x4B3A369A, 0x6690B1E6,
        0x2E62215F, 0x00E26164, 0x501C46E0, 0x475D27B4, 0x6A911520, 0x06D9E909, 0x2D9B3877, 0xBC356195,
        0x56E05697, 0x643195C6, 0x35F014E7, 0xC444E9FA, 0x21703217, 0xF97587B3, 0x697B49B5, 0x907B280F,
        0xA36413AD, 0x2A20E050, 0x45E5E981, 0x7D25A844, 0x5D749B0F, 0xB98939BC, 0x590374EA, 0x1034D153
    },
    {
        0x3D04C251, 0x3F98F25F, 0x54751A07, 0x1669D50B, 0x4320F791, 0x5E57E88A, 0xF94F63D6, 0xD3B541A6,
        0xA355933C, 0xA61895B8, 0x0C2B9227, 0x58210343, 0x840B8AD6, 0x9A6372A3, 0xB904D9F9, 0x6A192A09,
        0x194F3CB9, 0x82C7E5F8, 0x9F0D6809, 0x787D25A7, 0x5E2856D4, 0xB04C1C52, 0xA1A5B496, 0xA08076A2,
        0x83B3666A, 0xC4E0A03C, 0x70ED814C, 0x0B810057, 0x04C0512A, 0x5D430A80, 0x367F697C, 0x269EF04F,
        0x36C95427, 0x3B65E425, 0x108D8C30, 0x090740B7, 0x8B75B498, 0xE9B6A974, 0x5AC8249B, 0x79405615,
        0x89E007B6, 0x6280B84D, 0x19277A6C, 0x05BC6A98, 0x4B3D449F, 0x750F9C7D, 0x90433E46, 0xC201B786,
        0x36F17500, 0x1536E4F0, 0x384D024B, 0x8C9A9908, 0x0914D43E, 0x446B819F, 0x6A49504F, 0x8C089906,
        0x1F2A7B5F, 0x1D2626F7, 0xD41CC41A, 0x00E2C844, 0x181D3E0C, 0x6002CC70, 0x16429A64, 0x902996E2,
        0x3841D352, 0x5C9535B2, 0x78D15093, 0x0F7176E5, 0x69670A88, 0xB0118D07, 0x2863784A, 0x97305910,
        0x9062973E, 0x01ED922A, 0x438B0014, 0x28B6D1B4, 0x0E820306, 0xB5A96F75, 0x2A19794E, 0x0E54668D,
        0x124E5CC5, 0x7000B902, 0x32A3982F, 0xC965008E, 0x4296E0A6, 0xA99B7746, 0x5FD13110, 0x6273B250,
        0x9E7E010C, 0x05E59F1C, 0x18894101, 0x2320C906, 0xA8252277, 0xED7F819E, 0x7F26E6C2, 0x416999F9,
        0x45E0B342, 0xA77B5490, 0xB66D254C, 0x9330D6C8, 0xF9436750, 0x0C301984, 0x89D91024, 0x1404C742,
        0x4E70B342, 0x550346E2, 0x5E035D1A, 0x34A774A0, 0x535E8E56, 0xB8D85B33, 0x32A469D5, 0x76E84C1C,
        0x5F687A13, 0x4E948483, 0xBD918079, 0x9388B576, 0x3C4F8F45, 0x0F0BE8E4, 0x1C137159, 0x56F0D8AA,
        0x7090885F, 0x98A535F2, 0x18A375E9, 0x3D7581D1, 0x6B0468E8, 0xA2790E5A, 0x310D6782, 0xD4C6C7E2,
        0x4165682C, 0xA07A23E6, 0x03848E1D, 0x1C4D4E88, 0x24C19859, 0x8159E5D4, 0xD0C5BD65, 0x743D9B06,
        0x2B9223D6, 0x803F2787, 0x2427847F, 0x752179E0, 0x42967F5C, 0x3F67E83B, 0x65F79D3A, 0x923C4697,
        0xD4F30E30, 0x882586D9, 0xC3AF2954, 0x2485C0A0, 0x367A8532, 0x7E31D648, 0x01138806, 0xB29D558F,
        0xC0387532, 0x446B831F, 0x2320D682, 0x93F56054, 0x87A890F0, 0x31899147, 0xC37A8C84, 0xB2B67E2F,
        0x3764831B, 0x01905322, 0x7CC5D46B, 0x67C85C02, 0xF94B6F71, 0x21272F21, 0x170EC43B, 0x1CE0490E,
        0x33B10A88, 0x0562D710, 0x8A2A70F4, 0x103E4804, 0x5D848991, 0xC0904E07, 0x18B4A6A8, 0x9B87F1E4,
        0x6739B9F4, 0x0E6D8412, 0x10300A17, 0x9E21A4B0, 0xD6F86B14, 0x7620AF89, 0x6ED0D23F, 0x2D9739D1,
        0xD90E6D84, 0x5EB23812, 0x32A5AF4C, 0x0EBB38D4, 0xB2AA77EF, 0x5E841D8D, 0x10C9950A, 0x486B8C4C,
        0x320C9227, 0x6EB86C02, 0x73C9697C, 0x6779A8ED, 0xD29668D6, 0x9270B342, 0xC02B900D, 0x3D720993,
        0x4B3D2B52, 0x70A11F1F, 0x63346F6F, 0x00E2C8A4, 0x6F499E05, 0x127C2114, 0x98A5E4ED, 0x21175626,
        0x5F196883, 0x889B664C, 0x2C46ED82, 0x4B3C690E, 0x286BD0F2, 0x48B309A8, 0x5821ED92, 0x3E546C49,
        0x320875B7, 0x70C7509A, 0x45E8D62E, 0x93708D6F, 0x3841D4E3, 0xA26F824E, 0x5C956C21, 0x32A11100,
        0x475F22CC, 0xC0ACE8D3, 0x18A3D0B7, 0x90432BD1, 0x6E8AD62E, 0x4D0C92EF, 0x7A4F88D0, 0x32A4C2E4,
        0x83B7E4F2, 0x3D29704F, 0x3B6C9002, 0x9330B5ED, 0x5F196883, 0xC0ACE8D3, 0x34A774A0, 0x5D848991,
        0x2D9739D1, 0x4E70B342, 0xB66D254C, 0x3C4F8F45, 0x127C2114, 0xA2790E5A, 0x486B8C4C, 0xC0ACE8D3,
        0x34A774A0, 0x5D848991, 0x2D9739D1, 0x4E70B342, 0xB66D254C, 0x3C4F8F45, 0x127C2114, 0xA2790E5A
    },
    {
        0x97305910, 0x9062973E, 0x01ED922A, 0x438B0014, 0x28B6D1B4, 0x0E820306, 0xB5A96F75, 0x2A19794E,
        0x0E54668D, 0x124E5CC5, 0x7000B902, 0x32A3982F, 0xC965008E, 0x4296E0A6, 0xA99B7746, 0x5FD13110,
        0x6273B250, 0x9E7E010C, 0x05E59F1C, 0x18894101, 0x2320C906, 0xA8252277, 0xED7F819E, 0x7F26E6C2,
        0x416999F9, 0x45E0B342, 0xA77B5490, 0xB66D254C, 0x9330D6C8, 0xF9436750, 0x0C301984, 0x89D91024,
        0x1404C742, 0x4E70B342, 0x550346E2, 0x5E035D1A, 0x34A774A0, 0x535E8E56, 0xB8D85B33, 0x32A469D5,
        0x76E84C1C, 0x5F687A13, 0x4E948483, 0xBD918079, 0x9388B576, 0x3C4F8F45, 0x0F0BE8E4, 0x1C137159,
        0x56F0D8AA, 0x7090885F, 0x98A535F2, 0x18A375E9, 0x3D7581D1, 0x6B0468E8, 0xA2790E5A, 0x310D6782,
        0xD4C6C7E2, 0x4165682C, 0xA07A23E6, 0x03848E1D, 0x1C4D4E88, 0x24C19859, 0x8159E5D4, 0xD0C5BD65,
        0x743D9B06, 0x2B9223D6, 0x803F2787, 0x2427847F, 0x752179E0, 0x42967F5C, 0x3F67E83B, 0x65F79D3A,
        0x923C4697, 0xD4F30E30, 0x882586D9, 0xC3AF2954, 0x2485C0A0, 0x367A8532, 0x7E31D648, 0x01138806,
        0xB29D558F, 0xC0387532, 0x446B831F, 0x2320D682, 0x93F56054, 0x87A890F0, 0x31899147, 0xC37A8C84,
        0xB2B67E2F, 0x3764831B, 0x01905322, 0x7CC5D46B, 0x67C85C02, 0xF94B6F71, 0x21272F21, 0x170EC43B,
        0x1CE0490E, 0x33B10A88, 0x0562D710, 0x8A2A70F4, 0x103E4804, 0x5D848991, 0xC0904E07, 0x18B4A6A8,
        0x9B87F1E4, 0x6739B9F4, 0x0E6D8412, 0x10300A17, 0x9E21A4B0, 0xD6F86B14, 0x7620AF89, 0x6ED0D23F,
        0x2D9739D1, 0xD90E6D84, 0x5EB23812, 0x32A5AF4C, 0x0EBB38D4, 0xB2AA77EF, 0x5E841D8D, 0x10C9950A,
        0x486B8C4C, 0x320C9227, 0x6EB86C02, 0x73C9697C, 0x6779A8ED, 0xD29668D6, 0x9270B342, 0xC02B900D,
        0x3D720993, 0x4B3D2B52, 0x70A11F1F, 0x63346F6F, 0x00E2C8A4, 0x6F499E05, 0x127C2114, 0x98A5E4ED,
        0x21175626, 0x5F196883, 0x889B664C, 0x2C46ED82, 0x4B3C690E, 0x286BD0F2, 0x48B309A8, 0x5821ED92,
        0x3E546C49, 0x320875B7, 0x70C7509A, 0x45E8D62E, 0x93708D6F, 0x3841D4E3, 0xA26F824E, 0x5C956C21,
        0x32A11100, 0x475F22CC, 0xC0ACE8D3, 0x18A3D0B7, 0x90432BD1, 0x6E8AD62E, 0x4D0C92EF, 0x7A4F88D0,
        0x32A4C2E4, 0x83B7E4F2, 0x3D29704F, 0x3B6C9002, 0x9330B5ED, 0x5F196883, 0xC0ACE8D3, 0x34A774A0,
        0x5D848991, 0x2D9739D1, 0x4E70B342, 0xB66D254C, 0x3C4F8F45, 0x127C2114, 0xA2790E5A, 0x486B8C4C,
        0x97305910, 0x9062973E, 0x01ED922A, 0x438B0014, 0x28B6D1B4, 0x0E820306, 0xB5A96F75, 0x2A19794E,
        0x0E54668D, 0x124E5CC5, 0x7000B902, 0x32A3982F, 0xC965008E, 0x4296E0A6, 0xA99B7746, 0x5FD13110,
        0x6273B250, 0x9E7E010C, 0x05E59F1C, 0x18894101, 0x2320C906, 0xA8252277, 0xED7F819E, 0x7F26E6C2,
        0x416999F9, 0x45E0B342, 0xA77B5490, 0xB66D254C, 0x9330D6C8, 0xF9436750, 0x0C301984, 0x89D91024,
        0x1404C742, 0x4E70B342, 0x550346E2, 0x5E035D1A, 0x34A774A0, 0x535E8E56, 0xB8D85B33, 0x32A469D5,
        0x76E84C1C, 0x5F687A13, 0x4E948483, 0xBD918079, 0x9388B576, 0x3C4F8F45, 0x0F0BE8E4, 0x1C137159,
        0x56F0D8AA, 0x7090885F, 0x98A535F2, 0x18A375E9, 0x3D7581D1, 0x6B0468E8, 0xA2790E5A, 0x310D6782,
        0xD4C6C7E2, 0x4165682C, 0xA07A23E6, 0x03848E1D, 0x1C4D4E88, 0x24C19859, 0x8159E5D4, 0xD0C5BD65,
        0x743D9B06, 0x2B9223D6, 0x803F2787, 0x2427847F, 0x752179E0, 0x42967F5C, 0x3F67E83B, 0x65F79D3A,
        0x923C4697, 0xD4F30E30, 0x882586D9, 0xC3AF2954, 0x2485C0A0, 0x367A8532, 0x7E31D648, 0x01138806
    },
    {
        0xF94F63D6, 0xD3B541A6, 0xA355933C, 0xA61895B8, 0x0C2B9227, 0x58210343, 0x840B8AD6, 0x9A6372A3,
        0xB904D9F9, 0x6A192A09, 0x194F3CB9, 0x82C7E5F8, 0x9F0D6809, 0x787D25A7, 0x5E2856D4, 0xB04C1C52,
        0xA1A5B496, 0xA08076A2, 0x83B3666A, 0xC4E0A03C, 0x70ED814C, 0x0B810057, 0x04C0512A, 0x5D430A80,
        0x367F697C, 0x269EF04F, 0x36C95427, 0x3B65E425, 0x108D8C30, 0x090740B7, 0x8B75B498, 0xE9B6A974,
        0x5AC8249B, 0x79405615, 0x89E007B6, 0x6280B84D, 0x19277A6C, 0x05BC6A98, 0x4B3D449F, 0x750F9C7D,
        0x90433E46, 0xC201B786, 0x36F17500, 0x1536E4F0, 0x384D024B, 0x8C9A9908, 0x0914D43E, 0x446B819F,
        0x6A49504F, 0x8C089906, 0x1F2A7B5F, 0x1D2626F7, 0xD41CC41A, 0x00E2C844, 0x181D3E0C, 0x6002CC70,
        0x16429A64, 0x902996E2, 0x3841D352, 0x5C9535B2, 0x78D15093, 0x0F7176E5, 0x69670A88, 0xB0118D07,
        0x2863784A, 0x97305910, 0x9062973E, 0x01ED922A, 0x438B0014, 0x28B6D1B4, 0x0E820306, 0xB5A96F75,
        0x2A19794E, 0x0E54668D, 0x124E5CC5, 0x7000B902, 0x32A3982F, 0xC965008E, 0x4296E0A6, 0xA99B7746,
        0x5FD13110, 0x6273B250, 0x9E7E010C, 0x05E59F1C, 0x18894101, 0x2320C906, 0xA8252277, 0xED7F819E,
        0x7F26E6C2, 0x416999F9, 0x45E0B342, 0xA77B5490, 0xB66D254C, 0x9330D6C8, 0xF9436750, 0x0C301984,
        0x89D91024, 0x1404C742, 0x4E70B342, 0x550346E2, 0x5E035D1A, 0x34A774A0, 0x535E8E56, 0xB8D85B33,
        0x32A469D5, 0x76E84C1C, 0x5F687A13, 0x4E948483, 0xBD918079, 0x9388B576, 0x3C4F8F45, 0x0F0BE8E4,
        0x1C137159, 0x56F0D8AA, 0x7090885F, 0x98A535F2, 0x18A375E9, 0x3D7581D1, 0x6B0468E8, 0xA2790E5A,
        0x310D6782, 0xD4C6C7E2, 0x4165682C, 0xA07A23E6, 0x03848E1D, 0x1C4D4E88, 0x24C19859, 0x8159E5D4,
        0xD0C5BD65, 0x743D9B06, 0x2B9223D6, 0x803F2787, 0x2427847F, 0x752179E0, 0x42967F5C, 0x3F67E83B,
        0x65F79D3A, 0x923C4697, 0xD4F30E30, 0x882586D9, 0xC3AF2954, 0x2485C0A0, 0x367A8532, 0x7E31D648,
        0x01138806, 0xB29D558F, 0xC0387532, 0x446B831F, 0x2320D682, 0x93F56054, 0x87A890F0, 0x31899147,
        0xC37A8C84, 0xB2B67E2F, 0x3764831B, 0x01905322, 0x7CC5D46B, 0x67C85C02, 0xF94B6F71, 0x21272F21,
        0x170EC43B, 0x1CE0490E, 0x33B10A88, 0x0562D710, 0x8A2A70F4, 0x103E4804, 0x5D848991, 0xC0904E07,
        0x18B4A6A8, 0x9B87F1E4, 0x6739B9F4, 0x0E6D8412, 0x10300A17, 0x9E21A4B0, 0xD6F86B14, 0x7620AF89,
        0x6ED0D23F, 0x2D9739D1, 0xD90E6D84, 0x5EB23812, 0x32A5AF4C, 0x0EBB38D4, 0xB2AA77EF, 0x5E841D8D,
        0x10C9950A, 0x486B8C4C, 0x320C9227, 0x6EB86C02, 0x73C9697C, 0x6779A8ED, 0xD29668D6, 0x9270B342,
        0xC02B900D, 0x3D720993, 0x4B3D2B52, 0x70A11F1F, 0x63346F6F, 0x00E2C8A4, 0x6F499E05, 0x127C2114,
        0x98A5E4ED, 0x21175626, 0x5F196883, 0x889B664C, 0x2C46ED82, 0x4B3C690E, 0x286BD0F2, 0x48B309A8,
        0x5821ED92, 0x3E546C49, 0x320875B7, 0x70C7509A, 0x45E8D62E, 0x93708D6F, 0x3841D4E3, 0xA26F824E,
        0x5C956C21, 0x32A11100, 0x475F22CC, 0xC0ACE8D3, 0x18A3D0B7, 0x90432BD1, 0x6E8AD62E, 0x4D0C92EF,
        0x7A4F88D0, 0x32A4C2E4, 0x83B7E4F2, 0x3D29704F, 0x3B6C9002, 0x9330B5ED, 0x5F196883, 0xC0ACE8D3,
        0x34A774A0, 0x5D848991, 0x2D9739D1, 0x4E70B342, 0xB66D254C, 0x3C4F8F45, 0x127C2114, 0xA2790E5A,
        0x486B8C4C, 0x97305910, 0x9062973E, 0x01ED922A, 0x438B0014, 0x28B6D1B4, 0x0E820306, 0xB5A96F75,
        0x2A19794E, 0x0E54668D, 0x124E5CC5, 0x7000B902, 0x32A3982F, 0xC965008E, 0x4296E0A6, 0xA99B7746
    }
};

/**
 * @brief Performs the Feistel function (F) used in each round of Blowfish.
 * @details Takes a 32-bit half-block, splits it into four 8-bit octets, and 
 *          processes them through four S-boxes using addition and XOR.
 * @param context Pointer to the Blowfish context containing the S-boxes.
 * @param half The 32-bit value to process.
 * @return The 32-bit result of the Feistel function.
 */
static inline uint32_t feistel_function(const Blowfish* context, uint32_t half)
{
    // Break the 32-bit input into four 8-bit chunks (bytes).
    uint8_t first_octet = (uint8_t)(half >> 24);
    uint8_t second_octet = (uint8_t)(half >> 16);
    uint8_t third_octet = (uint8_t)(half >> 8);
    uint8_t fourth_octet = (uint8_t)(half);

    // Blowfish F-function: ((S1[a] + S2[b]) ^ S3[c]) + S4[d].
    // Addition is performed modulo 2^32.
    uint32_t result = context->substitution_boxes[0][first_octet] +
        context->substitution_boxes[1][second_octet];

    result ^= context->substitution_boxes[2][third_octet];
    result += context->substitution_boxes[3][fourth_octet];

    return result;
}

/**
 * @brief Initializes the Blowfish P-array and S-boxes with a secret key.
 * @details Sets the initial hex values (derived from pi), XORs the P-array with 
 *          the key, and then performs a self-encryption process to generate 
 *          unique subkeys.
 * @param context Pointer to the Blowfish structure to be initialized.
 * @param secret_key Pointer to the byte array containing the user key.
 * @param length Length of the secret key in bytes.
 */
void initialize(Blowfish* context, const uint8_t* secret_key, size_t length)
{
    if (context == NULL || secret_key == NULL || length < 1 || length > 256)
    {
        return;
    }

    // Step 1: Initialize P-array with the digits of PI.
    for (int index = 0; index < 18; index++)
    {
        context->permutation_array[index] = initial_permuation_values[index];
    }

    // Step 2: Initialize all four S-boxes with the digits of PI.
    for (int box_index = 0; box_index < 4; box_index++)
    {
        for (int index = 0; index < 256; index++)
        {
            context->substitution_boxes[box_index][index] = initial_substitution_values[box_index][index];
        }
    }

    // Step 3: XOR the P-array with the user-provided key.
    int key_byte_offset = 0;

    for (int xor_index = 0; xor_index < 18; xor_index++)
    {
        uint32_t word = 0;

        // Build a 32-bit word from the key bytes (cycling through the key if short).
        for (int byte_shift = 0; byte_shift < 4; byte_shift++)
        {
            word = (word << 8) | secret_key[key_byte_offset];

            key_byte_offset = (key_byte_offset + 1) % length;
        }

        context->permutation_array[xor_index] ^= word;
    }

    // Step 4: Gradually replace all P-array and S-box entries with encrypted data.
    uint32_t left_block = 0x00000000;
    uint32_t right_block = 0x00000000;

    // Scramble the P-array entries (18 entries total, 2 per encryption call).
    for (int p_scramble_index = 0; p_scramble_index < 18; p_scramble_index += 2)
    {
        blowfish_encrypt_block(context, &left_block, &right_block);

        context->permutation_array[p_scramble_index] = left_block;
        context->permutation_array[p_scramble_index + 1] = right_block;
    }

    // Scramble all S-box entries (4 boxes * 256 entries = 1024 total).
    for (int s_scramble_index = 0; s_scramble_index < 4; s_scramble_index++)
    {
        for (int entry_scramble = 0; entry_scramble < 256; entry_scramble += 2)
        {
            blowfish_encrypt_block(context, &left_block, &right_block);

            context->substitution_boxes[s_scramble_index][entry_scramble] = left_block;
            context->substitution_boxes[s_scramble_index][entry_scramble + 1] = right_block;
        }
    }
}

/**
 * @brief Encrypts a single 64-bit block of data.
 * @details Splits the block into two 32-bit halves and performs 16 rounds 
 *          of Feistel networking followed by a final output transformation.
 * @param context Pointer to the initialized Blowfish context.
 * @param left_half Pointer to the higher 32 bits of the data block.
 * @param right_half Pointer to the lower 32 bits of the data block.
 */
void blowfish_encrypt_block(const Blowfish* context, uint32_t* left_half, uint32_t* right_half)
{
    if (context == NULL || left_half == NULL || right_half == NULL)
    {
        return;
    }

    uint32_t left = *left_half;
    uint32_t right = *right_half;
    uint32_t temp = 0;

    // Execute 16 rounds of the Feistel Network.
    for (int round = 0; round < 16; round++)
    {
        // XOR the left half with the P-array subkey for this round.
        left ^= context->permutation_array[round];

        // XOR the right half with the result of the Feistel function on the left.
        right ^= feistel_function(context, left);

        // Swap left and right halves for the next round.
        temp = left;
        left = right;
        right = temp;
    }

    // Undo the final swap from the loop.
    temp = left;
    left = right;
    right = temp;

    // Apply the final P-array XORs (the "Output Transoformation").
    right ^= context->permutation_array[16];
    left ^= context->permutation_array[17];

    // Store results back into the provided pointers.
    *left_half = left;
    *right_half = right;
}

/**
 * @brief Decrypts a single 64-bit block of data.
 * @details Reverses the steps of the encryption process by applying the 
 *          P-array subkeys in descending order (17 down to 0).
 * @param context Pointer to the initialized Blowfish context.
 * @param left_half Pointer to the higher 32 bits of the ciphertext block.
 * @param right_half Pointer to the lower 32 bits of the ciphertext block.
 */
void blowfish_decrypt_block(const Blowfish* context, uint32_t* left_half, uint32_t* right_half)
{
    if (context == NULL || left_half == NULL || right_half == NULL)
    {
        return;
    }

    uint32_t left = *left_half;
    uint32_t right = *right_half;
    uint32_t temp = 0;

    // Initialize XORs use subkeys in reverse order (17 and 16).
    left ^= context->permutation_array[17];
    right ^= context->permutation_array[16];

    // Initial swap to align the reversed Feistel rounds.
    temp = left;
    left = right;
    right = temp;

    // Execute 16 rounds in reverse (from index 15 down to 0).
    for (int round = 15; round >= 0; round--)
    {
        // Standar Feistel swap.
        temp = left;
        left = right;
        right = temp;

        // Perform the inverse math: Feistel function then P-array XOR.
        right ^= feistel_function(context, left);
        left ^= context->permutation_array[round];
    }

    // Store decrypted halves back into the provided pointers.
    *left_half = left;
    *right_half = right;
}