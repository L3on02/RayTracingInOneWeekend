#ifndef __SKIP_INTERNAL_FATBINARY_HEADERS
#include "fatbinary_section.h"
#endif
#define __CUDAFATBINSECTION  ".nvFatBinSegment"
#define __CUDAFATBINDATASECTION  ".nv_fatbin"
asm(
".section .nv_fatbin, \"a\"\n"
".align 8\n"
"fatbinData:\n"
".quad 0x00100001ba55ed50,0x00000000000003a0,0x0000005801010002,0x0000000000000348\n"
".quad 0x0000000000000345,0x0000005000010007,0x0000001600000040,0x0000000000002013\n"
".quad 0x0000000000000000,0x0000000000000918,0x445543656b614d43,0x656c69706d6f4341\n"
".quad 0x000075632e644972,0x010102464c457fa2,0x0002660001000733,0xa8210001007a00be\n"
".quad 0x0704e80031000708,0x00500550000ef500,0x0040000200380040,0x68732e000001000f\n"
".quad 0x2e00626174727473,0xf100086d79270008,0x0078646e68735f05,0x2e7466752e766e2e\n"
".quad 0xf4000e7972746e65,0x642e006f666e6902,0x6172665f67756265,0x6e696c40000d656d\n"
".quad 0x57000f5f16002265,0x770014737361735f,0x127478745f787470,0x65725f4c004e0000\n"
".quad 0x7435001805002a67,0x6262618600627079,0x6900232e00766572,0x0011616c65722e00\n"
".quad 0x0f63616d32000c02,0x6c61632e766ec100,0x000e68706172676c,0x00566f746f727052\n"
".quad 0x612e7f003f766e20,0xff01146e6f697463,0x004d8c0400010f04,0x0001000500030000\n"
".quad 0x0100062c00185911,0x00072c00186d1100,0x0a2c0018af110001,0x2c0018bd11000100\n"
".quad 0x0018da110001000b,0x18e9110001000c2c,0x05200001000d2c00,0x0001000e2c001801\n"
".quad 0x000010e003341611,0x01000a0efb010100,0x001a0f000d010101,0x762e0df100010409\n"
".quad 0x38206e6f69737265,0x677261742e00322e,0x30385f6d73207465,0x2e0002f00169202c\n"
".quad 0x5f73736572646461,0x35343620657a6973,0x091a015265732100,0x1d0001007b2c01b7\n"
".quad 0x290f01b401002b7d,0x1e7b1101e0041100,0x130825001101c000,0x040e1b0610080305\n"
".quad 0xf100010000d97f11,0x666e65676c01081b,0x3620474445203a65,0x614d43000400342e\n"
".quad 0x6f4341445543656b,0x644972656c69706d,0x2f02f0002f75632e,0x616d6c2f656d6f68\n"
".quad 0x72547961522f6761,0x4f6e496705f10067,0x6e656b656557656e,0x2f646c6975622f64\n"
".quad 0x73656c6946d60044,0x2f312e32322e332f,0x010012005b00004d,0x0008ffffffff4000\n"
".quad 0x0008fd130008fe13,0x010073fffffffc65,0x0036050025116f00,0x0e021801112c0001\n"
".quad 0x3d00010040220001,0x0001080030000114,0x01542f0400400b1f,0x0a01751315130040\n"
".quad 0xd811001568130001,0x9b09100024020006,0x0018260001001201,0x1e000100012e00a8\n"
".quad 0x1f0200800f009540,0x40001a2f0c00404d,0x1f040040591f0b00,0x00406d1f1400405a\n"
".quad 0x400f040003742604,0x130400407f1f0900,0x0c0040001f06d823,0x40af1f2c0040971f\n"
".quad 0x0b004000102f0c00,0x403313040040bd1f,0x1f0b004000832f00,0x3004b62f040040da\n"
".quad 0x5b000518011f0300,0x1300010070000001,0x00010020220040b8,0x002700170305ca04\n"
".quad 0x0000400b1f054008,0x08081b040100d81f,0x089004005c061300,0x170008701b00010c\n"
".quad 0x501c00380f00c808, 0x0000000000000000\n"
".text\n");
#ifdef __cplusplus
extern "C" {
#endif
extern const unsigned long long fatbinData[118];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
static const __fatBinC_Wrapper_t __fatDeviceText __attribute__ ((aligned (8))) __attribute__ ((section (__CUDAFATBINSECTION)))= 
	{ 0x466243b1, 1, fatbinData, 0 };
#ifdef __cplusplus
}
#endif