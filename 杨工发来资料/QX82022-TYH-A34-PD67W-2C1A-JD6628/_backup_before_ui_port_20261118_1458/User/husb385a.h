#ifndef __HUSB385A_DRIVER_INTERFACE_H
#define __HUSB385A_DRIVER_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iic_soft.h"

typedef enum
{
	HU_PDO_20W,
	HU_PDO_30W,
	HU_PDO_45W,
	HU_PDO_45WAVS,
	HU_PDO_65W,
} HUSY385A_PDO;

#pragma pack(1)
// HUSB385A 0xC0~0xCC 寄存器位域结构体（13字节，无填充）
// 严格匹配手册寄存器定义，支持按位访问每个字段
typedef struct {
    // 地址0xC0: VOLTAGE REFERENCE1 电压参考低8位
    uint8_t Vref_L;

    // 地址0xC1: VOLTAGE REFERENCE2 电压参考高2位
    union {
        uint8_t Vref_9_8;
        struct {
            uint8_t Reserved1 : 6;
            uint8_t Vref_H : 2;
        } bit;
    } Vref_H;

    // 地址0xC2: STATUS0 端口插入状态
    union {
        uint8_t STATUS0;
        struct {
            uint8_t Sink_insert_a2 : 1;
            uint8_t Sink_insert_a1 : 1;
            uint8_t Sink_insert_c2 : 1;
            uint8_t Sink_insert_c1 : 1;
            uint8_t Reserved2_0 : 4;
        } bit;
    } STATUS0;

    // 地址0xC3: STATUS1 协议协商状态
    union {
        uint8_t STATUS1;
        struct {
            uint8_t PD_contract : 4;
            uint8_t DPDM_conract : 4;
        } bit;
    } STATUS1;

    // 地址0xC4: STATUS2 核心运行状态
    union {
        uint8_t STATUS2;
        struct {
            uint8_t Reserved5_0 : 2;
            uint8_t OCP_comp : 1;
            uint8_t Reserved4_0 : 1;
            uint8_t Link_out : 1;
            uint8_t Reserved3_0 : 1;
            uint8_t AMS_succ : 1;
            uint8_t AMS_process : 1;
        } bit;
    } STATUS2;

    // 地址0xC5: 保留寄存器
    uint8_t Reserved_byte5;

    // 地址0xC6: STATUS3 温度/异常标志
    union {
        uint8_t STATUS3;
        struct {
            uint8_t Reserved7_0 : 4;
            uint8_t OTW1_flag : 1;
            uint8_t OTW2_flag : 1;
            uint8_t Cold_flag : 1;
            uint8_t Reserved6_0 : 1;
        } bit;
    } STATUS3;

    // 地址0xC7: VBUS_MEA VBUS电压采样
    uint8_t Vbus_mea;

    // 地址0xC8: IBUS_SUM_MEA 总电流采样
    uint8_t Ibus_sum_mea;

    // 地址0xC9: IBUS_C1_MEA C1口电流采样
    uint8_t Ibus_C1_mea;

    // 地址0xCA: IBUS_C2/A2_MEA C2/A2口电流采样
    uint8_t Ibus_C2_A2_mea;

    // 地址0xCB: IBUS_A1_MEA A1口电流采样
    uint8_t Ibus_A1_mea;

    // 地址0xCC: V_NTC_MEA NTC电压采样
    uint8_t V_ntc_mea;  

} HUSB385A_Stat_Reg;

typedef union {
    uint8_t buff[13];
    HUSB385A_Stat_Reg reg;
} HUSB385A_Stat_Union;

typedef enum
{
	APPLE24,
	DCP,
	UFCS,
	QC20,
	QC30,
	QC30P,
	FCP,
	SCP,
	AFC,
	PE11,
	PE20,
} ENUM_PROTOCOL_MODE;

typedef struct {
	uint16_t vref;
	uint8_t insert_c1;
	uint8_t insert_c2;
	uint8_t insert_a1;
	uint8_t insert_a2;
	ENUM_PROTOCOL_MODE protocol;
	enum
	{
		PD5V,
		PD9V,
		PD12V,
		PD20V,
		PPS1,
		PPS2,
		AVS1,
		AVS2,
		V50,
		detach,
	} pd_contract;
	uint8_t ocp;
	uint8_t linkout;
	uint8_t ams_succ;
	uint8_t ams_status;
	enum
	{
		OTW_NONE,
		OTW1,
		OTW2,
		OTW_COLD,
	} otw_flag;
	uint8_t vbus; // 0.1v
	uint16_t ibus_sum; //0.1a
	uint16_t ibus_c1; //0.1a
	uint16_t ibus_c2_a2; //0.1a
	uint16_t ibus_a1; //0.1a
	uint8_t vntc; // 0.01v
} HUSB385A_Stat_t;
#pragma pack()

#define IIC_REG_SIZE 24
#define DSI_REG_INFO HUSB385A_Stat_t

uint8_t husb385a_read_status(HUSB385A_Stat_t *stat);
void husb385a_setPdo(HUSY385A_PDO pdo);
void husb385a_init(void);

#endif
