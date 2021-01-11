#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// �������
#define ACT_ENABLE_POWER				1	// ��������� �����
#define ACT_DISABLE_POWER				2	// ���������� �����
#define ACT_CLR_FAULT					3	// ������� ���� fault
#define ACT_CLR_WARNING					4	// ������� ���� warning

#define ACT_DBG_PULSE_SYNC				50	// ��������� ������� � ����� SYNC_1
#define ACT_DBG_SET_CURRENT_RANGE		51	// ��������� ��������� �� ���� (0, 1, 2)
#define ACT_DBG_POWER_SUPPLY_EN			52	// �������� ���������� ���������� ������� �� 500�

#define ACT_CONFIG_UNIT					100	// ������������ �����
#define ACT_SOFTWARE_START				101	// ����� ���������
#define ACT_STOP_PROCESS				102	// ���� ���������

#define ACT_SAVE_TO_ROM					200	// ���������� ���������������� ������ �� FLASH ����������
#define ACT_RESTORE_FROM_ROM			201	// �������������� ������ �� FLASH
#define ACT_RESET_TO_DEFAULT			202	// ����� DataTable � ��������� �� ���������

#define ACT_BOOT_LOADER_REQUEST			320	// ���������� ���������� � ����� ��������������������
// -----------------------------

// ��������
// ����������� ��������
#define REG_CURRENT_LEVEL_RANGE0		0	// ������� ����� �� ���� ��������� 0 (� * 10)
#define REG_CURRENT_LEVEL_RANGE1		1	// ������� ����� �� ���� ��������� 1 (� * 10)
#define REG_SHUNT_RESISTANCE			2	// ������������� ����� (����)
#define REG_FAN_CTRL					5	// ��������� ���������� ������������
#define REG_AFTER_PULSE_PAUSE			6	// ����� ����� �������� (��)
#define REG_CURRENT_PER_CURBOARD		3	// ������������ ��������� ���� � ����� CurrentBoard (�)
#define REG_CURBOARD_QUANTITY			4	// ���������� CurrentBoard
// 5 - 9
#define REG_I_TO_DAC_RANGE0_K			2	// �������� 0 - ����������� ��������� � �������� ���
#define REG_I_TO_DAC_RANGE0_B			3	// �������� 0 - �������� ��������� � �������� ���
#define REG_I_TO_DAC_RANGE1_K			2	// �������� 1 - ����������� ��������� � �������� ���
#define REG_I_TO_DAC_RANGE1_B			3	// �������� 1 - �������� ��������� � �������� ���
#define REG_I_TO_DAC_RANGE2_K			2	// �������� 2 - ����������� ��������� � �������� ���
#define REG_I_TO_DAC_RANGE2_B			3	// �������� 2 - �������� ��������� � �������� ���

#define REG_ADC_VOLTAGE_K				44	// ����������� ������ ���������� K
#define REG_ADC_VOLTAGE_B				45	// �������� ������ ���������� B
//
#define REG_ADC_I_RANGE0_P2				51	// �������� 0 - ����������� ������ ���������� �2 �1�6
#define REG_ADC_I_RANGE0_P1				52	// �������� 0 - ����������� ������ ���������� �1 x1000
#define REG_ADC_I_RANGE0_P0				53	// �������� 0 - �������� ������ ���������� �0
#define REG_ADC_I_RANGE0_N				54	// �������� 0 - ��������� ������������ ������ ����������
#define REG_ADC_I_RANGE0_D				55	// �������� 0 - ����������� ������������ ������ ����������
#define REG_ADC_I_RANGE0_B				56	// �������� 0 - �������� ������ ���������� B
//
#define REG_ADC_I_RANGE1_P2				57	// �������� 1 - ����������� ������ ���������� �2 �1�6
#define REG_ADC_I_RANGE1_P1				58	// �������� 1 - ����������� ������ ���������� �1 x1000
#define REG_ADC_I_RANGE1_P0				59	// �������� 1 - �������� ������ ���������� �0
#define REG_ADC_I_RANGE1_N				60	// �������� 1 - ��������� ������������ ������ ����������
#define REG_ADC_I_RANGE1_D				61	// �������� 1 - ����������� ������������ ������ ����������
#define REG_ADC_I_RANGE1_B				62	// �������� 1 - �������� ������ ���������� B
//
#define REG_ADC_I_RANGE2_P2				63	// �������� 2 - ����������� ������ ���������� �2 �1�6
#define REG_ADC_I_RANGE2_P1				64	// �������� 2 - ����������� ������ ���������� �1 x1000
#define REG_ADC_I_RANGE2_P0				65	// �������� 2 - �������� ������ ���������� �0
#define REG_ADC_I_RANGE2_N				66	// �������� 2 - ��������� ������������ ������ ����������
#define REG_ADC_I_RANGE2_D				67	// �������� 2 - ����������� ������������ ������ ����������
#define REG_ADC_I_RANGE2_B				68	// �������� 2 - �������� ������ ���������� B
//
#define REG_REGULATOR_RANGE0_Kp			80	// �������� 0 - ���������������� ����������� ����������
#define REG_REGULATOR_RANGE0_Ki			81	// �������� 0 - ������������ ����������� ����������
#define REG_REGULATOR_RANGE1_Kp			80	// �������� 1 - ���������������� ����������� ����������
#define REG_REGULATOR_RANGE1_Ki			81	// �������� 1 - ������������ ����������� ����������
#define REG_REGULATOR_RANGE2_Kp			80	// �������� 2 - ���������������� ����������� ����������
#define REG_REGULATOR_RANGE2_Ki			81	// �������� 2 - ������������ ����������� ����������

#define REG_CURRENT_PULSE_VALUE			128	// ������� ��������� �������� ���� (� * 10)

// ������������ �������� ������-������
#define REG_DBG							150	// ������� ������ �������

// �������� ������ ������
#define REG_DEV_STATE					192	// ������� ���������
#define REG_FAULT_REASON				193	// ������� Fault
#define REG_DISABLE_REASON				194	// ������� Disable
#define REG_WARNING						195	// ������� Warning
#define REG_PROBLEM						196	// ������� Problem
#define REG_OP_RESULT					197	// ������� ���������� ��������
#define REG_SUB_STATE					198	// ������� ���������������� ���������

#define REG_RESULT_CURRENT				200	// ����������� �������� �������� ���� (� * 10)
#define REG_BATTERY_VOLTAGE				201	// ���������� �� ������� ������������� (� * 10)
// -----------------------------
#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record


// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

//  Fault and disable codes
#define DF_NONE							0

// Problem
#define PROBLEM_NONE					0
#define PROBLEM_FOLOWING_ERROR			1

//  Warning
#define WARNING_NONE					0
#define WARNING_CURRENT_CUTOFF			1

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  ���������� �������� �� ������
#define ERR_OPERATION_BLOCKED			2	//  �������� �� ����� ���� ��������� � ������� ��������� ����������
#define ERR_DEVICE_NOT_READY			3	//  ���������� �� ������ ��� ����� ���������
#define ERR_WRONG_PWD					4	//  ������������ ����

// Endpoints
#define EP_CURRENT						1
#define EP_REGULATOR_ERR				2

#endif //  __DEV_OBJ_DIC_H
