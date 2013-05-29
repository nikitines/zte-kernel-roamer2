/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_lead.h"
#include <mach/gpio.h>
#ifdef CONFIG_ZTE_PLATFORM
#include <mach/zte_memlog.h>
#endif
static struct dsi_buf lead_tx_buf;
static struct dsi_buf lead_rx_buf;
extern u32 LcdPanleID;
static int lcd_bkl_ctl=97;
#define GPIO_LCD_RESET 129

static bool onewiremode = false;

/*ic define*/
#define HIMAX_8363 		1
#define HIMAX_8369 		2
#define NOVATEK_35510	3

#define HIMAX8369_TIANMA_TN_ID		0xB1
#define HIMAX8369_TIANMA_IPS_ID		0xA5
#define HIMAX8369_LEAD_ID				0
#define HIMAX8369_LEAD_HANNSTAR_ID	0x88
#define NT35510_YUSHUN_ID				0
#define NT35510_LEAD_ID				0xA0

/*about icchip sleep and display on */
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
//static char exit_sleep[2] = {0x11, 0x00};
//static char display_on[2] = {0x29, 0x00};
static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};


/**************************************
trulylg HX8357C initial
***************************************/
static char hx8357c_trulylg_para_0xb9[4]={0xB9,0xff,0x83,0x57};  
static char hx8357c_trulylg_para_0xb3[5]={0xb3,0x43,0x00,0x06,0x06}; 
static char hx8357c_trulylg_para_0x3a[2]={0x3a,0x77};  
static char hx8357c_trulylg_para_0xe9[2]={0xe9,0x20};
static char hx8357c_trulylg_para_0xcc[2]={0xcc,0x07};  
static char hx8357c_trulylg_para_0xb6[2]={0xb6,0x50};  
static char hx8357c_trulylg_para_0xb1[7]={0xb1,0x00,0x15,0x1c,0x1c,0x83,0xaa};  
static char hx8357c_trulylg_para_0xb4[8]={0xb4,0x02,0x40,0x00,0x2a,0x2a,0x0d,0x4f}; 
static char hx8357c_trulylg_para_0xb0[3]={0xb0,0x68,0x01};  
static char hx8357c_trulylg_para_0xc0[7]={0xc0,0x33,0x50,0x01,0x7c,0x1e,0x08}; 
static char hx8357c_trulylg_para_0xba[17] = {0xba,0x00,0x56,0xd4,0x00,0x0a,0x00,0x10,0x32,0x6e,0x04,0x05,0x9a,0x14,0x19,0x10,0x40};
static char hx8357c_trulylg_para_0xe0[68]={0xe0,0x00,0x00,0x02,0x00,0x0A,0x00,0x11,0x00,0x1D,0x00,0x23,0x00,0x35,0x00,0x41,0x00,0x4B,0x00,0x4B,0x00,0x42,0x00,0x3A,0x00,0x27,0x00,0x1B,0x00,0x12,0x00,0x0C,0x00,0x03,0x01,0x02,0x00,0x0A,0x00,0x11,0x00,0x1D,0x00,0x23,0x00,0x35,0x00,0x41,0x00,0x4B,0x00,0x4B,0x00,0x42,0x00,0x3A,0x00,0x27,0x00,0x1B,0x00,0x12,0x00,0x0C,0x00,0x03,0x00,0x00}; 
static char hx8357c_trulylg_para_0x11[2]={0x11,0x00}; 
static char hx8357c_trulylg_para_0x29[2]={0x29,0x00}; 



static struct dsi_cmd_desc hx8357c_trulylg_display_on_cmds[] = 
{

	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_trulylg_para_0xb9), hx8357c_trulylg_para_0xb9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xb3), hx8357c_trulylg_para_0xb3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0x3a), hx8357c_trulylg_para_0x3a},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xe9), hx8357c_trulylg_para_0xe9},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xcc),hx8357c_trulylg_para_0xcc},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xb6), hx8357c_trulylg_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xb1), hx8357c_trulylg_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xb4), hx8357c_trulylg_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xb0), hx8357c_trulylg_para_0xb0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_trulylg_para_0xc0), hx8357c_trulylg_para_0xc0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_trulylg_para_0xba), hx8357c_trulylg_para_0xba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_trulylg_para_0xe0), hx8357c_trulylg_para_0xe0},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(hx8357c_trulylg_para_0x11), hx8357c_trulylg_para_0x11},	
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(hx8357c_trulylg_para_0x29), hx8357c_trulylg_para_0x29},	

};


/**************************************
CMI NT35310 initial
***************************************/

/*static char cmi_nt35310_page2_en[]={0xed,0x01,0xfe};
static char cmi_nt35310_para0[]={0xb0,0x0c};
static char cmi_nt35310_para1[]={0xB3,0x61};

static char cmi_nt35310_para2[]={0x21,0x00};                                
static char cmi_nt35310_para3[]={0x3A,0x67};                                
static char cmi_nt35310_para4[]={0x36,0x00};                                
                                
static char cmi_nt35310_para5[]={0x11,0x00};                                
static char cmi_nt35310_para6[]={0x29,0x00};                                

static struct dsi_cmd_desc nt35310_cmi_display_on_cmds[] = {  
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_nt35310_page2_en), cmi_nt35310_page2_en},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35310_para0), cmi_nt35310_para0},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35310_para1), cmi_nt35310_para1},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(cmi_nt35310_para2), cmi_nt35310_para2},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35310_para3), cmi_nt35310_para3},
	{DTYPE_GEN_WRITE1, 1, 0, 0, 0, sizeof(cmi_nt35310_para4), cmi_nt35310_para4},	
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(cmi_nt35310_para5), cmi_nt35310_para5},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(cmi_nt35310_para6), cmi_nt35310_para6},

};*/

static char cmi_35310_para01[]={0xED,0x01,0xFE};
static char cmi_35310_para02[]={0xEE,0xDE,0x21};
static char cmi_35310_para03[]={0xF1,0x01};
static char cmi_35310_para04[]={0xDF,0x10};
static char cmi_35310_para05[]={0xB7,0x20};
static char cmi_35310_para06[]={0xC0,0x44,0x44,0x10,0x10};
static char cmi_35310_para07[]={0xC2,0x44,0x44,0x44};
static char cmi_35310_para08[]={0xC4,0x3E};
static char cmi_35310_para09[]={0xC6,0x00,0xE2,0xE2,0xE2};
static char cmi_35310_para10[]={0xBF,0xAA};
static char cmi_35310_para11[]={0xB7,0x00,0x00,0x3F,0x00,0x5E,0x00,0x64,
								0x00,0x8C,0x00,0xAC,0x00,0xDC,0x00,0x70,
								0x00,0x90,0x00,0xEB,0x00,0xDC,0x00};
static char cmi_35310_para12[]={0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char cmi_35310_para13[]={0xBA,0x24,0x00,0x00,0x00};
static char cmi_35310_para14[]={0xC2,0x0A,0x00,0x04,0x00};
static char cmi_35310_para15[]={0xC7,0x00,0x00,0x00,0x00};
static char cmi_35310_para16[]={0xC9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00};
static char cmi_35310_para17[]={0xE0,0x00,0x00,0x0D,0x00,0x18,0x00,0x27,
								0x00,0x33,0x00,0x3D,0x00,0x4D,0x00,0x65,
								0x00,0x78,0x00,0x88,0x00,0x99,0x00,0xAF,
								0x00,0xC0,0x00,0xCB,0x00,0xDB,0x00,0xE5,
								0x00,0xEF,0x00,0xF3,0x00};
static char cmi_35310_para18[]={0xE1,0x00,0x00,0x0D,0x00,0x18,0x00,0x27,
								0x00,0x33,0x00,0x3D,0x00,0x4D,0x00,0x65,
								0x00,0x78,0x00,0x88,0x00,0x99,0x00,0xAF,
								0x00,0xC0,0x00,0xCB,0x00,0xDC,0x00,0xE5,
								0x00,0xEF,0x00,0xF3,0x00};
static char cmi_35310_para19[]={0xE2,0x00,0x00,0x0D,
								0x00,0x18,0x00,0x27,0x00,0x33,0x00,0x3D,
								0x00,0x4D,0x00,0x65,0x00,0x78,0x00,0x88,
								0x00,0x99,0x00,0xAF,0x00,0xC0,0x00,0xCB,
								0x00,0xDB,0x00,0xE5,0x00,0xEF,0x00,0xF3,
								0x00};
static char cmi_35310_para20[]={0xE3,0x00,0x00,0x0D,
								0x00,0x18,0x00,0x27,0x00,0x33,0x00,0x3D,
								0x00,0x4D,0x00,0x65,0x00,0x78,0x00,0x88,
								0x00,0x99,0x00,0xAF,0x00,0xC0,0x00,0xCB,
								0x00,0xDC,0x00,0xE5,0x00,0xEF,0x00,0xF3,
								0x00};
static char cmi_35310_para21[]={0xE4,0x00,0x00,0x0D,
								0x00,0x18,0x00,0x27,0x00,0x33,0x00,0x3D,
								0x00,0x4D,0x00,0x65,0x00,0x78,0x00,0x88,
								0x00,0x99,0x00,0xAF,0x00,0xC0,0x00,0xCB,
								0x00,0xDB,0x00,0xE5,0x00,0xEF,0x00,0xF3,
								0x00};
static char cmi_35310_para22[]={0xE5,0x00,0x00,0x0D,
								0x00,0x18,0x00,0x27,0x00,0x33,0x00,0x3D,
								0x00,0x4D,0x00,0x65,0x00,0x78,0x00,0x88,
								0x00,0x99,0x00,0xAF,0x00,0xC0,0x00,0xCB,
								0x00,0xDC,0x00,0xE5,0x00,0xEF,0x00,0xF3,
								0x00};
static char cmi_35310_para23[]={0xE6,0x12,0x00,0x33,
								0x00,0x44,0x00,0x55,0x00,0x55,0x00,0x88,
								0x00,0x99,0x00,0xCC,0x00,0xEE,0x00,0xCC,
								0x00,0xBB,0x00,0x99,0x00,0x98,0x00,0x56,
								0x00,0x32,0x00,0x00,0x00};
static char cmi_35310_para24[]={0xE7,0x12,0x00,0x33,
								0x00,0x44,0x00,0x55,0x00,0x55,0x00,0x88,
								0x00,0x99,0x00,0xCC,0x00,0xEE,0x00,0xCC,
								0x00,0xBB,0x00,0x99,0x00,0x98,0x00,0x56,
								0x00,0x32,0x00,0x00,0x00};
static char cmi_35310_para25[]={0xE8,0x12,0x00,0x33,
								0x00,0x44,0x00,0x55,0x00,0x55,0x00,0x88,
								0x00,0x99,0x00,0xCC,0x00,0xEE,0x00,0xCC,
								0x00,0xBB,0x00,0x99,0x00,0x98,0x00,0x56,
								0x00,0x32,0x00,0x00,0x00};
static char cmi_35310_para26[]={0xE9,0xAA,0x00,0x00,0x00};
static char cmi_35310_para27[]={0x00,0xAA};
static char cmi_35310_para28[]={0xF3,0x00};
static char cmi_35310_para29[]={0xF9,0x06,0x10,0x29,0x00};

static char cmi_35310_para_36[]={0x36,0x00};
static char cmi_35310_para_b0[]={0xb0,0x8c};
static char cmi_35310_para_b3[]={0xB3,0x61};

static char cmi_35310_para30[]={0x3A,0x66};
static char cmi_35310_para31[]={0x35,0x00};
static char cmi_35310_para32[]={0x11,0x00};
static char cmi_35310_para33[]={0x29,0x00};


static struct dsi_cmd_desc nt35310_cmi_display_on_cmds[] = {
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para01), cmi_35310_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para02), cmi_35310_para02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para03), cmi_35310_para03},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para04), cmi_35310_para04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para05), cmi_35310_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para06), cmi_35310_para06},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para07), cmi_35310_para07},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para08), cmi_35310_para08},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para09), cmi_35310_para09},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para10), cmi_35310_para10},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para11), cmi_35310_para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para12), cmi_35310_para12},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para13), cmi_35310_para13},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para14), cmi_35310_para14},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para15), cmi_35310_para15},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para16), cmi_35310_para16},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para17), cmi_35310_para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para18), cmi_35310_para18},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para19), cmi_35310_para19},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para20), cmi_35310_para20},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para21), cmi_35310_para21},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para22), cmi_35310_para22},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para23), cmi_35310_para23},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para24), cmi_35310_para24},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para25), cmi_35310_para25},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para26), cmi_35310_para26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para27), cmi_35310_para27},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para28), cmi_35310_para28},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cmi_35310_para29), cmi_35310_para29},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para_36), cmi_35310_para_36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para_b0), cmi_35310_para_b0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para_b3), cmi_35310_para_b3},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para30), cmi_35310_para30},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cmi_35310_para31), cmi_35310_para31},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(cmi_35310_para32), cmi_35310_para32},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(cmi_35310_para33), cmi_35310_para33}, 
		
};


/**************************************
TianMa HX8357C initial
***************************************/


static char hx8357c_tianma_para_0xb9[]={0xB9,0xff,0x83,0x57};  
static char hx8357c_tianma_para_0xb3[]={0xb3,0x43,0x00,0x06,0x06}; 
static char hx8357c_tianma_para_0xe3[] = {0xE3, 0x17, 0x0F};
static char hx8357c_tianma_para_0x3a[]={0x3a,0x07};  
static char hx8357c_tianma_para_0xe9[]={0xe9,0x20};
static char hx8357c_tianma_para_0xcc[]={0xcc,0x07};  
static char hx8357c_tianma_para_0xb6[]={0xb6,0x1d};  
static char hx8357c_tianma_para_0xb1[] = {0xB1, 0x00, 0x14, 0x1E, 0x1E, 0x83, 0xAA};
static char hx8357c_tianma_para_0xb4[] = {0xb4,0x02,0x40,0x00,0x2a,0x2a,0x0d,0x4f};
static char hx8357c_tianma_para_0xb0[]={0xb0,0x68,0x01};  
static char hx8357c_tianma_para_0xc0[] = {0xc0,0x33,0x50,0x01,0x7c,0x1e,0x08};
static char hx8357c_tianma_para_0xba[] = {0xba,0x00,0x56,0xd4,0x00,0x0a,0x00,0x10,0x32,0x6e,0x04,0x05,0x9a,0x14,0x19,0x10,0x40};
static char hx8357c_tianma_para_0xe0[] = {0xE0, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x1A, 0x00, 0x20, 0x00, 0x34, 0x00, 0x41, 0x00, 0x4F, 0x00, 0x45, 0x00, 0x3A, 0x00, 0x2E, 0x00, 0x18, 0x00, 0x17, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x1A, 0x00, 0x20, 0x00, 0x34, 0x00, 0x41, 0x00, 0x4F, 0x00, 0x45, 0x00, 0x3A, 0x00, 0x2E, 0x00, 0x18, 0x00, 0x17, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03};
static char hx8357c_tianma_para_0x11[]={0x11,0x00}; 
static char hx8357c_tianma_para_0x29[]={0x29,0x00};

static struct dsi_cmd_desc hx8357c_tianma_display_on_cmds[] = 
{
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_tianma_para_0xb9), hx8357c_tianma_para_0xb9},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xb3), hx8357c_tianma_para_0xb3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0x3a), hx8357c_tianma_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xe3),hx8357c_tianma_para_0xe3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xe9), hx8357c_tianma_para_0xe9},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xcc),hx8357c_tianma_para_0xcc},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xb6), hx8357c_tianma_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xb1), hx8357c_tianma_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xb4), hx8357c_tianma_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xb0), hx8357c_tianma_para_0xb0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8357c_tianma_para_0xc0), hx8357c_tianma_para_0xc0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_tianma_para_0xba), hx8357c_tianma_para_0xba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 10, sizeof(hx8357c_tianma_para_0xe0), hx8357c_tianma_para_0xe0},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(hx8357c_tianma_para_0x11), hx8357c_tianma_para_0x11},	
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(hx8357c_tianma_para_0x29), hx8357c_tianma_para_0x29},	
};


void myudelay(unsigned int usec)
{
udelay(usec);
}
static void lcd_panle_reset(void)
{
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(10);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(50);
}


#ifdef CONFIG_ZTE_PLATFORM
static u32 __init get_lcdpanleid_from_bootloader(void)
{
	smem_global*	msm_lcd_global = (smem_global*) ioremap(SMEM_LOG_GLOBAL_BASE, sizeof(smem_global));
	
	printk("debug chip id 0x%x\n",msm_lcd_global->lcd_id);
	
	if (((msm_lcd_global->lcd_id) & 0xffff0000) == 0x09830000) 
	{
		
		switch(msm_lcd_global->lcd_id & 0x0000ffff)
		{	
			case 0x0001:
				return (u32)LCD_PANEL_3P5_HX8357C_TRULY_LG;
			case 0x0002:
				return (u32)LCD_PANEL_3P5_HX8357C_TIANMA;
			case 0x0003:
				return (u32)LCD_PANEL_3P5_NT35310_CMI;
			case 0x0004:
				return 4;
			case 0x0005:
				return 5;
			case 0x0006:
				return 6;
			case 0x0007:
				return 7;
			default:
				break;
		}		
	}
	return (u32)LCD_PANEL_NOPANEL;
}
#endif

static int mipi_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	mipi_set_tx_power_mode(1);

	mipi_dsi_cmds_tx(mfd, &lead_tx_buf, display_off_cmds,
			ARRAY_SIZE(display_off_cmds));
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(5);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
	gpio_direction_output(121,0);
	return 0;
}

//added by zte_gequn091966,20110428
static void select_1wire_mode(void)
{
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(120);
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(280);				////ZTE_LCD_LUYA_20100226_001
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(650);				////ZTE_LCD_LUYA_20100226_001
	
}


static void send_bkl_address(void)
{
	unsigned int i,j;
	i = 0x72;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_address \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}

static void send_bkl_data(int level)
{
	unsigned int i,j;
	i = level & 0x1F;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_data \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}
static void mipi_zte_set_backlight(struct msm_fb_data_type *mfd)
{
       /*value range is 1--32*/
	 int current_lel = mfd->bl_level;
  	 unsigned long flags;


    	printk("[ZYF] lcdc_set_bl level=%d, %d\n", 
		   current_lel , mfd->panel_power_on);

    	if(!mfd->panel_power_on)
	{
    		gpio_direction_output(lcd_bkl_ctl, 0);
		mdelay(3);
		onewiremode = FALSE;
	    	return;
    	}

    	if(current_lel < 1)
    	{
        	current_lel = 0;
   	 }
		
    	if(current_lel > 32)
    	{
        	current_lel = 32;
    	}
    
    	local_irq_save(flags);
		
   	if(current_lel==0)
    	{
    		gpio_direction_output(lcd_bkl_ctl, 0);
		mdelay(3);
		onewiremode = FALSE;
			
    	}
    	else 
	{
		if(!onewiremode)	
		{
			printk("[LY] before select_1wire_mode\n");
			select_1wire_mode();
			onewiremode = TRUE;
		}
		send_bkl_address();
		send_bkl_data(current_lel-1);

	}
		
    	local_irq_restore(flags);
}



static int first_time_panel_on = 1;
static int mipi_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd = platform_get_drvdata(pdev);
	printk("gequn mipi_lcd_on\n");

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if(first_time_panel_on){
		first_time_panel_on = 0;
		return 0;
	}

	lcd_panle_reset();
	printk("mipi init start\n");
	mipi_set_tx_power_mode(1);
	switch(LcdPanleID){			
		case (u32)LCD_PANEL_3P5_HX8357C_TRULY_LG:
				mipi_dsi_cmds_tx(mfd, &lead_tx_buf, hx8357c_trulylg_display_on_cmds,ARRAY_SIZE(hx8357c_trulylg_display_on_cmds));
				printk("LCD_PANEL_3P5_HX8357C_TRULY_LG init ok !!\n");
				break;	
						
		case (u32)LCD_PANEL_3P5_HX8357C_TIANMA:
				mipi_dsi_cmds_tx(mfd, &lead_tx_buf, hx8357c_tianma_display_on_cmds,ARRAY_SIZE(hx8357c_tianma_display_on_cmds));
				printk("LCD_PANEL_3P5_HX8357C_TIANMA init ok !!\n");
				break;
		case (u32)LCD_PANEL_3P5_NT35310_CMI:
				mipi_dsi_cmds_tx(mfd, &lead_tx_buf, nt35310_cmi_display_on_cmds,ARRAY_SIZE(nt35310_cmi_display_on_cmds));
				printk("LCD_PANEL_3P5_NT35310_CMI init ok !!\n");
				break;				
		default:
			    mipi_dsi_cmds_tx(mfd, &lead_tx_buf, hx8357c_trulylg_display_on_cmds,ARRAY_SIZE(hx8357c_trulylg_display_on_cmds));
				printk("can't get icpanelid value\n");
				break;
				
	}	
	mipi_set_tx_power_mode(0);
	return 0;
}



static struct msm_fb_panel_data lead_panel_data = {
	.on		= mipi_lcd_on,
	.off		= mipi_lcd_off,
	.set_backlight = mipi_zte_set_backlight,
};



static int ch_used[3];

int mipi_lead_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

printk(KERN_ERR
		  "%s: gequn mipi_device_register n855!\n", __func__);
	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_lead", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	lead_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &lead_panel_data,
		sizeof(lead_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}


static int __devinit mipi_lead_lcd_probe(struct platform_device *pdev)
{	

	struct msm_panel_info   *pinfo =&( ((struct msm_fb_panel_data  *)(pdev->dev.platform_data))->panel_info);
	if (pdev->id == 0) return 0;
	
	mipi_dsi_buf_alloc(&lead_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&lead_rx_buf, DSI_BUF_SIZE);
	
#ifdef CONFIG_ZTE_PLATFORM	
	if((LcdPanleID = get_lcdpanleid_from_bootloader() )==(u32)LCD_PANEL_NOPANEL)
		printk("cann't get get_lcdpanelid from bootloader\n");
#endif	
	if (LcdPanleID ==LCD_PANEL_3P5_NT35310_CMI)//this panel is different from others
	{
		pinfo->lcdc.h_back_porch = 30;
		pinfo->lcdc.h_front_porch = 30;
		pinfo->lcdc.h_pulse_width = 30;
		pinfo->lcdc.v_back_porch = 12;
		pinfo->lcdc.v_front_porch = 10;
		pinfo->lcdc.v_pulse_width = 10;
	}


	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_lead_lcd_probe,
	.driver = {
		.name   = "mipi_lead",
	},
};

static int __init mipi_lcd_init(void)
{
   		printk("gequn mipi_lcd_init\n");

	return platform_driver_register(&this_driver);
}

module_init(mipi_lcd_init);
