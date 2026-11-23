/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "debug.h"
#include "husb385a.h"

/* Private macro -------------------------------------------------------------*/
#define IIC_DEVICE_ADDR 0x34
//#define DEBUG_HUSB
#ifdef DEBUG_HUSB
#define LOGHU LOG
#define LOGHU_HEX LOG_HEX
#else
#define LOGHU(...)
#define LOGHU_HEX(...)
#endif
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t pdo65[0xAF - 0xA4 + 1] = {
    0x2C,   // 0xA4 Pdo9v_cur_cfg[7:0]    3A=30?0x1E? ??:3A=3000mA?300 LSB?0x12C??8?0x2C
    0x2C,   // 0xA5 Pdo12v_cur_cfg[7:0]   3A?0x12C?0x2C
    0x2C,   // 0xA6 Pdo15v_cur_cfg[7:0]   3A?0x12C?0x2C
    0x45,   // 0xA7 Pdo20v_cur_cfg[7:0]   3.25A=325?0x145?0x45
    0x41,   // 0xA8 Apdo1_cur_cfg         3.25A=60 LSB(50mA)?0x3C
    0x00,   // 0xA9 Apdo2_cur_cfg         0
    0xbe,   // 0xAA [7:6]5V3A,[5]pdo9[4]pdo12[3]pdo15[2]pdo20[1]3A[0]emark
    0xd2,   // 0xAB Apdo1_max_v_cfg       20V
    0x00,   // 0xAC Apdo2_max_v_cfg       0
    0x12,   // 0xAD Power_max(QC)         18w
		0x00,   // 0xAE [7] reserved [6:5] vref, [4]AVSEN, [3]0:AVS15V, 1:AVS20V, [2] GPIOOUTMODE [1] GPIOOUT [0] RESERVED
		0x00,   // 0xAE [7] Spr_avs_en_dps [6] Spr_avs_max_volt_dps, [5:4]reserved, [3]Disable DPDM, [2:1] Reserved [0] Vref_lsb_sel
};
const uint8_t pdo45[0xAF - 0xA4 + 1] = {
    0x2C,   // 0xA4 9V3A?0x2C
    0x2C,   // 0xA5 12V3A?0x2C
    0x2C,   // 0xA6 15V3A?0x2C
    0xE1,   // 0xA7 20V2.25A=225?0xE1
    0x28,   // 0xA8 PPS1 2A=40 LSB?0x28
    0x00,   // 0xA9 PPS2??
    0xba,   // 0xAA [7:6]pdo5,[5]pdo9[4]pdo12[3]pdo15[2]pdo20[1]3A[0]emark
    0xd3,   // 0xAB Apdo1_max_v_cfg
    0x00,   // 0xAC Apdo2_max_v_cfg
    0x12,   // 0xAD Power_max(QC)         18w
		0x00,   // 0xAE [7] reserved [6:5] vref, [4]AVSEN, [3]0:AVS15V, 1:AVS20V, [2] GPIOOUTMODE [1] GPIOOUT [0] RESERVED
		0x00,   // 0xAE [7] Spr_avs_en_dps [6] Spr_avs_max_volt_dps, [5:4]reserved, [3]Disable DPDM, [2:1] Reserved [0] Vref_lsb_sel
};
//45(5V3A 9V3A 12V3A 15V3A 20V2.25A PPS:5-11V 4.5A
const uint8_t pdo45avs[0xAF - 0xA4 + 1] = {
    0x2C,   // 0xA4 9V3A?0x2C
    0x2C,   // 0xA5 12V3A?0x2C
    0x2C,   // 0xA6 15V3A?0x2C
    0xE1,   // 0xA7 20V2.25A=225?0xE1
    0x5A,   // 0xA8 PPS1 4.5A
    0x00,   // 0xA9 PPS2 none
    0xfa,   // 0xAA [7:6]pdo5,[5]pdo9[4]pdo12[3]pdo15[2]pdo20[1]3A[0]emark
    0x6e,   // 0xAB Apdo1_max_v_cfg
    0x00,   // 0xAC Apdo2_max_v_cfg 0
    0x12,   // 0xAD Power_max(QC)         18w
		0x18,   // 0xAE AVS20V [7] reserved [6:5] vref, [4]AVSEN, [3]0:AVS15V, 1:AVS20V, [2] GPIOOUTMODE [1] GPIOOUT [0] RESERVED
		0xc0,   // 0xAE [7] Spr_avs_en_dps [6] Spr_avs_max_volt_dps, [5:4]reserved, [3]Disable DPDM, [2:1] Reserved [0] Vref_lsb_sel
};
//30W(5V3A 9V3A 12V2.5A 15V2A 20V1.5A PPS:5-11V 3A
const uint8_t pdo30[0xAF - 0xA4 + 1] = {
    0x2C,   // 0xA4 9V3A?0x2C
    0xFA,   // 0xA5 12V3A?0x2C
    0xC8,   // 0xA6 15V2A?0x2C
    0x96,   // 0xA7 20V1.5A=225?0xE1
    0x3C,   // 0xA8 PPS1 3A
    0x00,   // 0xA9 PPS2 none
    0xE2,   // 0xAA [7:6]pdo5,[5]pdo9[4]pdo12[3]pdo15[2]pdo20[1]3A[0]emark
    0x6e,   // 0xAB Apdo1_max_v_cfg
    0x00,   // 0xAC Apdo2_max_v_cfg 0
    0x12,   // 0xAD Power_max(QC)         18w
		0x00,   // 0xAE AVS20V [7] reserved [6:5] vref, [4]AVSEN, [3]0:AVS15V, 1:AVS20V, [2] GPIOOUTMODE [1] GPIOOUT [0] RESERVED
		0x00,   // 0xAE [7] Spr_avs_en_dps [6] Spr_avs_max_volt_dps, [5:4]reserved, [3]Disable DPDM, [2:1] Reserved [0] Vref_lsb_sel
};
const uint8_t pdo20[0xAF - 0xA4 + 1] = {
    0xDE,   // 0xA4 9V 2.22A
    0xA7,   // 0xA5 12V 1.67A
    0x00,   // 0xA6 15V 0
    0x00,   // 0xA7 20V 0
    0x28,   // 0xA8 PPS1 2A=40 LSB?0x28
    0x00,   // 0xA9 PPS2 0
		0xc2,   // 0xAA [7:6]5V3A,[5]pdo9[4]pdo12[3]pdo15[2]pdo20[1]3A[0]emark
    0x6e,   // 0xAB Apdo1_max_v_cfg 11V
    0x00,   // 0xAC Apdo2_max_v_cfg
    0x12,   // 0xAD Power_max(QC)         18w
		0x00,   // 0xAE [7] reserved [6:5] vref, [4]AVSEN, [3]0:AVS15V, 1:AVS20V, [2] GPIOOUTMODE [1] GPIOOUT [0] RESERVED
		0x00,   // 0xAE [7] Spr_avs_en_dps [6] Spr_avs_max_volt_dps, [5:4]reserved, [3]Disable DPDM, [2:1] Reserved [0] Vref_lsb_sel
};

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @retval int
  */
void husb385a_init(void)
{
	I2c_Init();
}
/**
  * @brief  Main program.
  * @retval int
  */
void husb385a_setPdo(HUSY385A_PDO pdo)
{
	uint8_t *ppdo;
	uint8_t pdp;
	switch(pdo)
	{
		case HU_PDO_20W:
			pdp = 20;
			ppdo = (uint8_t*)pdo20;
			break;
		case HU_PDO_30W:
			pdp = 30;
			ppdo = (uint8_t*)pdo30;
			break;
		case HU_PDO_45W:
			pdp = 45;
			ppdo = (uint8_t*)pdo45;
			break;
		case HU_PDO_45WAVS:
			pdp = 45;
			ppdo = (uint8_t*)pdo45avs;
			break;
		case HU_PDO_65W:
			pdp = 65;
			ppdo = (uint8_t*)pdo65;
			break;
	}

	// write max power
	SI2C_Write_Bytes(IIC_DEVICE_ADDR, 0xba, &pdp, 1);
	// write pdo
	SI2C_Write_Bytes(IIC_DEVICE_ADDR, 0xa4, ppdo, 12);
	// force disconnect to make pdo work
	pdp = 0x80;
	SI2C_Write_Bytes(IIC_DEVICE_ADDR, 0xa0, &pdp, 1);
}

uint8_t husb385a_read_status(HUSB385A_Stat_t *stat)
{
	HUSB385A_Stat_Union reg;
	uint8_t ret = SI2C_Read_Bytes(IIC_DEVICE_ADDR, 0xc0, reg.buff, 0x0c);
	LOGHU("385a 0xc2: %x, %d\n", reg.buff[2], ret);
	if (ret == 0)
	{
		stat->vref = reg.reg.Vref_H.bit.Vref_H << 8 | reg.reg.Vref_L;
		stat->insert_a1 = reg.reg.STATUS0.bit.Sink_insert_a1;
		stat->insert_a2 = reg.reg.STATUS0.bit.Sink_insert_a2;
		stat->insert_c1 = reg.reg.STATUS0.bit.Sink_insert_c1;
		stat->insert_c2 = reg.reg.STATUS0.bit.Sink_insert_c2;
		stat->protocol = reg.reg.STATUS1.bit.DPDM_conract;
		stat->pd_contract = reg.reg.STATUS1.bit.PD_contract;
		stat->ocp = reg.reg.STATUS2.bit.OCP_comp;
		stat->linkout = reg.reg.STATUS2.bit.Link_out;
		stat->ams_succ = reg.reg.STATUS2.bit.AMS_succ;
		stat->ams_status = reg.reg.STATUS2.bit.AMS_process;
		stat->otw_flag = reg.reg.STATUS3.bit.Cold_flag? OTW_COLD : reg.reg.STATUS3.bit.OTW1_flag ? OTW1 : reg.reg.STATUS3.bit.OTW2_flag? OTW2 : OTW_NONE;
		stat->vbus = reg.reg.Vbus_mea;
		stat->ibus_sum = reg.reg.Ibus_sum_mea;
		stat->ibus_a1 = reg.reg.Ibus_A1_mea;
		stat->ibus_c1 = reg.reg.Ibus_C1_mea;
		stat->ibus_c2_a2 = reg.reg.Ibus_C2_A2_mea;
		stat->vntc = reg.reg.V_ntc_mea;
		LOGHU_HEX(reg.buff, 0x0c);
    LOGHU("VREF:%u,C1:%d,C2:%d,A1:%d,A2:%d,PROTO:%d,PD:%d,OCP:%d,LO:%d,AMS_SUCC:%d,AMS:%d,OTW:%d,VBUS:%d,ISUM:%d,IC1:%d,IC2A2:%d,IA1:%d,NTC:%d\r\n",
        stat->vref,
        stat->insert_c1, stat->insert_c2, stat->insert_a1, stat->insert_a2,
        stat->protocol, stat->pd_contract,
        stat->ocp, stat->linkout, stat->ams_succ, stat->ams_status, stat->otw_flag,
        stat->vbus,
        stat->ibus_sum,
        stat->ibus_c1,
        stat->ibus_c2_a2,
        stat->ibus_a1,
        stat->vntc);
	}
	return ret;
}
