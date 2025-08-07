# LCSU Control Board Software Specification

## Version Information
- **Branch**: develop_range
- **Commit**: 6c1035cdd5f1020aac2018a4314b1c1b6f30f4f1
- **Last Commit Message**: Исправлено выставление количества точек и построение синусоиды в зависимости от времени; длительность линейного падения после импульса выставлено как 20 мс

## Overview
This document provides a comprehensive specification of the LCSU Control Board firmware, including writable registers, read-only registers, commands, and endpoints.

## Data Types
- **Int16U**: 16-bit unsigned integer (uint16)
- **Int32U**: 32-bit unsigned integer (uint32)
- **float**: 32-bit floating point
- **Boolean**: Boolean type (0 = false, 1 = true)

---

## 1. Writable Registers

### Device Configuration

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 0 | REG_CURRENT_THRESHOLD_HIGH | float | Current threshold between ranges 1 and 2 (A) | 0 - 1100 | CURRENT_THRESHOLD_MIN to CURRENT_THRESHOLD_MAX_HIGH | Current threshold for range switching |
| 1 | REG_BATTERY_VOLTAGE_THRESHOLD | float | Battery charge threshold (V) | 10 - 50 | V_BAT_THRESHOLD_MIN to V_BAT_THRESHOLD_MAX | Battery voltage threshold setting |
| 2 | REG_DAC_OUTPUT_LIMIT_VALUE | float | DAC output limitation (0 - 4095) | 0 - 4095 | DAC_OUTPUT_LIM_MIN to DAC_OUTPUT_LIM_MAX | DAC output limit value |
| 3 | REG_BATTERY_FULL_CHRAGE_TIMEOUT | float | Full charge timeout (ms) | 30000 - 60000 | FULL_CHARGE_TIMEOUT_MIN to FULL_CHARGE_TIMEOUT_MAX | Full charge timeout setting |
| 4 | REG_BATTERY_RECHARGE_TIMEOUT | float | Recharge timeout after pulse (ms) | 0 - 40000 | RECHARGE_TIMEOUT_MIN to RECHARGE_TIMEOUT_MAX | Recharge timeout setting |
| 5 | REG_SHUNT_RESISTANCE | float | Shunt resistance (μΩ) | 0 - 5000 | SHUNT_RESISTANCE_MIN to SHUNT_RESISTANCE_MAX | Shunt resistance for current measurement |
| 6 | REG_REGULATOR_QI_MAX | float | Integral component level limitation | 0 - 65535 | 0 to INT16U_MAX | Regulator integral limit |
| 7 | REG_FAN_CTRL | float | Fan control enable | 0, 1 | NO, YES | Fan control enable/disable |
| 8 | REG_EXT_IND_CTRL | float | External indicator control enable | 0, 1 | NO, YES | External indicator control enable/disable |
| 9 | REG_AFTER_PULSE_PAUSE | float | Pause after pulse (ms) | 0 - 40000 | AFTER_PULSE_PAUSE_MIN to AFTER_PULSE_PAUSE_MAX | Post-measurement delay |
| 10 | REG_CURRENT_PER_CURBOARD | float | Max current amplitude per CurrentBoard (A) | 50 - 1500 | I_PER_CURBOARD_MIN to I_PER_CURBOARD_MAX | Current per board setting |
| 11 | REG_CURBOARDS | float | Number of CurrentBoard modules | 1 - 6 | CURBOARDS_MIN to CURBOARDS_MAX | Number of current boards |
| 12 | REG_SCOPE_STEP | float | Scope saving step | 0 - 65535 | 0 to INT16U_MAX | Scope step configuration |
| 13 | REG_FAN_OPERATE_PERIOD | float | Fan operation period (sec) | 0 - 600 | FAN_PERIOD_MIN to FAN_PERIOD_MAX | Fan operation period |
| 14 | REG_FAN_OPERATE_TIME | float | Fan operation time (sec) | 0 - 600 | FAN_TIME_MIN to FAN_TIME_MAX | Fan operation duration |
| 15 | REG_DAC_OFFSET | float | DAC signal offset | 0 - 4095 | DAC_OFFSET_MIN to DAC_OFFSET_MAX | DAC offset setting |
| 16 | REG_CONFIG_READY_STATE_TIMEOUT | float | Config ready state timeout (ms) | 1000 - 10000 | CONF_STATE_TIMEOUT_MIN to CONF_STATE_TIMEOUT_MAX | Configuration timeout |
| 17 | REG_TRAPEZE_CURRENT_RATE | float | dI/dt for trapezoidal pulse (A/μs) | 1 - 10 | TRAPEZE_CUR_RATE_MIN to TRAPEZE_CUR_RATE_MAX | Trapezoidal current rate |
| 19 | REG_PULSE_SHAPE | float | Pulse shape | 0, 1, 2 | SINE_SHAPE, MOD_SINE_SHAPE, TRAPEZE_SHAPE | Pulse shape selection |
| 60 | REG_CFG_NODE_ID | float | CAN Node ID configuration | 0 - 65535 | 0 to INT16U_MAX | CAN node ID setting |
| 63 | REG_CURRENT_THRESHOLD_LOW | float | Current threshold between ranges 0 and 1 (A) | 0 - 400 | CURRENT_THRESHOLD_MIN to CURRENT_THRESHOLD_MAX_LOW | Low current threshold |
| 120 | REG_PCB_VERSION | float | PCB version | 0, 1 | PCB_VERSION_10, PCB_VERSION_11 | PCB version selection |

### DAC Calibration Parameters (Range 0)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 20 | REG_DAC_I_RANGE0_P2 | float | Range 0 - Fine tuning coefficient P2 | -32768 - 32767 | COEF_P2_MIN to COEF_P2_MAX | DAC range 0 P2 coefficient |
| 21 | REG_DAC_I_RANGE0_P1 | float | Range 0 - Fine tuning coefficient P1 | -32768 - 32767 | COEF_P1_MIN to COEF_P1_MAX | DAC range 0 P1 coefficient |
| 22 | REG_DAC_I_RANGE0_P0 | float | Range 0 - Fine tuning coefficient P0 | -32768 - 32767 | COEF_P0_MIN to COEF_P0_MAX | DAC range 0 P0 coefficient |
| 23 | REG_DAC_I_RANGE0_K | float | Range 0 - DAC conversion coefficient K | -32768 - 32767 | COEF_K_MIN to COEF_K_MAX | DAC range 0 K coefficient |
| 24 | REG_DAC_I_RANGE0_B | float | Range 0 - DAC conversion offset B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | DAC range 0 B offset |

### DAC Calibration Parameters (Range 1)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 25 | REG_DAC_I_RANGE1_P2 | float | Range 1 - Fine tuning coefficient P2 | -32768 - 32767 | COEF_P2_MIN to COEF_P2_MAX | DAC range 1 P2 coefficient |
| 26 | REG_DAC_I_RANGE1_P1 | float | Range 1 - Fine tuning coefficient P1 | -32768 - 32767 | COEF_P1_MIN to COEF_P1_MAX | DAC range 1 P1 coefficient |
| 27 | REG_DAC_I_RANGE1_P0 | float | Range 1 - Fine tuning coefficient P0 | -32768 - 32767 | COEF_P0_MIN to COEF_P0_MAX | DAC range 1 P0 coefficient |
| 28 | REG_DAC_I_RANGE1_K | float | Range 1 - DAC conversion coefficient K | -32768 - 32767 | COEF_K_MIN to COEF_K_MAX | DAC range 1 K coefficient |
| 29 | REG_DAC_I_RANGE1_B | float | Range 1 - DAC conversion offset B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | DAC range 1 B offset |

### DAC Calibration Parameters (Range 2)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 64 | REG_DAC_I_RANGE2_P2 | float | Range 2 - Fine tuning coefficient P2 | -32768 - 32767 | COEF_P2_MIN to COEF_P2_MAX | DAC range 2 P2 coefficient |
| 65 | REG_DAC_I_RANGE2_P1 | float | Range 2 - Fine tuning coefficient P1 | -32768 - 32767 | COEF_P1_MIN to COEF_P1_MAX | DAC range 2 P1 coefficient |
| 66 | REG_DAC_I_RANGE2_P0 | float | Range 2 - Fine tuning coefficient P0 | -32768 - 32767 | COEF_P0_MIN to COEF_P0_MAX | DAC range 2 P0 coefficient |
| 67 | REG_DAC_I_RANGE2_K | float | Range 2 - DAC conversion coefficient K | -32768 - 32767 | COEF_K_MIN to COEF_K_MAX | DAC range 2 K coefficient |
| 68 | REG_DAC_I_RANGE2_B | float | Range 2 - DAC conversion offset B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | DAC range 2 B offset |

### ADC Calibration Parameters

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 30 | REG_ADC_VOLTAGE_K | float | Battery voltage measurement proportional coefficient K | -32768 - 32767 | V_BAT_K_MIN to V_BAT_K_MAX | ADC voltage K coefficient |
| 31 | REG_ADC_VOLTAGE_B | float | Battery voltage measurement offset coefficient B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | ADC voltage B offset |
| 32 | REG_K_AMP_RANGE0 | float | Current amplification coefficient channel 0 | -32768 - 32767 | -INT16S_MAX to INT16S_MAX | Current amp range 0 coefficient |
| 33 | REG_K_AMP_RANGE1 | float | Current amplification coefficient channel 1 | -32768 - 32767 | -INT16S_MAX to INT16S_MAX | Current amp range 1 coefficient |

### ADC Current Calibration (Range 0)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 34 | REG_ADC_I_RANGE0_1_P2 | float | Range 0 - Fine tuning coefficient P2 | -32768 - 32767 | COEF_P2_MIN to COEF_P2_MAX | ADC current range 0 P2 |
| 35 | REG_ADC_I_RANGE0_1_P1 | float | Range 0 - Fine tuning coefficient P1 | -32768 - 32767 | COEF_P1_MIN to COEF_P1_MAX | ADC current range 0 P1 |
| 36 | REG_ADC_I_RANGE0_1_P0 | float | Range 0 - Fine tuning coefficient P0 | -32768 - 32767 | COEF_P0_MIN to COEF_P0_MAX | ADC current range 0 P0 |
| 37 | REG_ADC_I_RANGE0_1_K | float | Range 0 - Coarse tuning coefficient K | -32768 - 32767 | COEF_K_MIN to COEF_K_MAX | ADC current range 0 K |
| 38 | REG_ADC_I_RANGE0_1_B | float | Range 0 - Coarse tuning offset B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | ADC current range 0 B |

### ADC Current Calibration (Range 1 & 2)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 39 | REG_ADC_I_RANGE1_P2 | float | Range 1&2 - Fine tuning coefficient P2 | -32768 - 32767 | COEF_P2_MIN to COEF_P2_MAX | ADC current range 1&2 P2 |
| 40 | REG_ADC_I_RANGE1_P1 | float | Range 1&2 - Fine tuning coefficient P1 | -32768 - 32767 | COEF_P1_MIN to COEF_P1_MAX | ADC current range 1&2 P1 |
| 41 | REG_ADC_I_RANGE1_P0 | float | Range 1&2 - Fine tuning coefficient P0 | -32768 - 32767 | COEF_P0_MIN to COEF_P0_MAX | ADC current range 1&2 P0 |
| 42 | REG_ADC_I_RANGE1_K | float | Range 1&2 - Coarse tuning coefficient K | -32768 - 32767 | COEF_K_MIN to COEF_K_MAX | ADC current range 1&2 K |
| 43 | REG_ADC_I_RANGE1_B | float | Range 1&2 - Coarse tuning offset B | -32768 - 32767 | OFFSET_MIN to OFFSET_MAX | ADC current range 1&2 B |

### Regulator Parameters (Range 0)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 44 | REG_REGULATOR_RANGE0_Kp | float | Range 0 - Proportional coefficient | -32768 - 32767 | REGULATOR_KP_MIN to REGULATOR_KP_MAX | Regulator range 0 Kp |
| 45 | REG_REGULATOR_RANGE0_Ki | float | Range 0 - Integral coefficient | -32768 - 32767 | REGULATOR_KI_MIN to REGULATOR_KI_MAX | Regulator range 0 Ki |

### Regulator Parameters (Range 1)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 46 | REG_REGULATOR_RANGE1_Kp | float | Range 1 - Proportional coefficient | -32768 - 32767 | REGULATOR_KP_MIN to REGULATOR_KP_MAX | Regulator range 1 Kp |
| 47 | REG_REGULATOR_RANGE1_Ki | float | Range 1 - Integral coefficient | -32768 - 32767 | REGULATOR_KI_MIN to REGULATOR_KI_MAX | Regulator range 1 Ki |

### Regulator Parameters (Range 2)

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 70 | REG_REGULATOR_RANGE2_Kp | float | Range 2 - Proportional coefficient | -32768 - 32767 | REGULATOR_KP_MIN to REGULATOR_KP_MAX | Regulator range 2 Kp |
| 71 | REG_REGULATOR_RANGE2_Ki | float | Range 2 - Integral coefficient | -32768 - 32767 | REGULATOR_KI_MIN to REGULATOR_KI_MAX | Regulator range 2 Ki |

### Regulator Control Parameters

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 50 | REG_REGULATOR_DELAY | float | Regulator activation delay (cycles) | 0 - 10 | REGULATOR_DELAY_MIN to REGULATOR_DELAY_MAX | Regulator delay setting |
| 51 | REG_REGULATOR_ALLOWED_ERR | float | Maximum allowed regulator error (%) | 0 - 15 | ALLOWED_ERR_MIN to ALLOWED_ERR_MAX | Allowed error percentage |
| 52 | REG_FOLLOWING_ERR_CNT | float | Maximum following error counter | 0 - 30 | FOLLOWING_ERR_CNT_MIN to FOLLOWING_ERR_CNT_MAX | Following error counter max |
| 53 | REG_FOLLOWING_ERR_MUTE | float | Following error mute | 0, 1 | NO, YES | Following error mute enable |
| 54 | REG_REF_VOLTAGE_VARIABLE | float | Variable reference voltage (mV) | 0 - 4000 | REF_VOLTAGE_MIN to REF_VOLTAGE_MAX | Reference voltage setting |

### Current Limits

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 61 | REG_LOW_ITM_LIMIT | float | Minimum allowed Itm value | 0 - 6500 | 0 to CURRENT_SETPOINT_MAX | Low current limit |
| 62 | REG_HIGH_ITM_LIMIT | float | Maximum allowed Itm value | 0 - 6500 | 0 to CURRENT_SETPOINT_MAX | High current limit |

### Runtime Parameters

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 128 | REG_CURRENT_PULSE_VALUE | float | Current pulse amplitude setting (A) | 30 - 6500 | CURRENT_SETPOINT_MIN to CURRENT_SETPOINT_MAX | Current pulse value |
| 129 | REG_TRAPEZE_DURATION | float | Trapezoidal pulse duration (ms) | 1 - 10 | TRAPEZE_DURATION_MIN to TRAPEZE_DURATION_MAX | Trapezoidal duration |
| 150 | REG_DBG | float | Debug mode register | -32768 - 32767 | -INT16S_MAX to INT16S_MAX | Debug mode setting |

---

## 2. Read-Only Registers

### Device Status

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 192 | REG_DEV_STATE | float | Device state register | 0 - 5 | DS_None, DS_Fault, DS_Disabled, DS_Ready, DS_ConfigReady, DS_InProcess | Device state |
| 193 | REG_FAULT_REASON | float | Fault reason register | 0, 1 | DF_NONE, DF_PROBLEM_BATTERY | Fault reason code |
| 194 | REG_DISABLE_REASON | float | Disable reason register | 0, 1 | DF_NONE, DF_PROBLEM_BATTERY | Disable reason code |
| 195 | REG_WARNING | float | Warning register | 0 | WARNING_NONE | Warning status |
| 196 | REG_PROBLEM | float | Problem register | 0, 1 | PROBLEM_NONE, PROBLEM_FOLLOWING_ERROR | Problem status |
| 197 | REG_OP_RESULT | float | Operation result register | 0, 1, 2 | OPRESULT_NONE, OPRESULT_OK, OPRESULT_FAIL | Operation result |
| 198 | REG_SUB_STATE | float | Sub-state register | 0 - 4 | SS_None, SS_PowerPrepare, SS_PulsePrepare, SS_WaitAfterPulse, SS_Pulse | Device sub-state |

### Measurement Results

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 200 | REG_RESULT_CURRENT | float | Achieved current pulse value (A) | 0 - 65535 | 0 to float max | Measured current value |
| 201 | REG_BATTERY_VOLTAGE | float | Battery capacitor voltage (V) | 0 - 65535 | 0 to float max | Battery voltage measurement |

### Firmware Information

| Num | Name | Type | Description | Value List/Range | Value Name | Value Description |
|-----|------|------|-------------|------------------|-------------|-------------------|
| 256 | REG_FWINFO_SLAVE_NID | float | Device CAN slave node ID | 0 - 65535 | 0 to INT16U_MAX | Slave node ID |
| 257 | REG_FWINFO_MASTER_NID | float | Device CAN master node ID | 0 - 65535 | 0 to INT16U_MAX | Master node ID |
| 260 | REG_FWINFO_STR_LEN | float | Information string record length | 0 - 65535 | 0 to INT16U_MAX | String length |
| 261 | REG_FWINFO_STR_BEGIN | float | Beginning of information string record | 0 - 65535 | 0 to INT16U_MAX | String start address |

---

## 3. Commands

### Power Control Commands

| Num | Name | Description |
|-----|------|-------------|
| 1 | ACT_ENABLE_POWER | Enable power supply block |
| 2 | ACT_DISABLE_POWER | Disable power supply block |

### System Control Commands

| Num | Name | Description |
|-----|------|-------------|
| 3 | ACT_CLR_FAULT | Clear all fault states |
| 4 | ACT_CLR_WARNING | Clear all warning states |
| 100 | ACT_CONFIG_UNIT | Configure the unit |
| 101 | ACT_SOFTWARE_START | Start measurement process |
| 102 | ACT_STOP_PROCESS | Stop measurement process |

### Data Management Commands

| Num | Name | Description |
|-----|------|-------------|
| 200 | ACT_SAVE_TO_ROM | Save user data to processor FLASH |
| 201 | ACT_RESTORE_FROM_ROM | Restore data from FLASH |
| 202 | ACT_RESET_TO_DEFAULT | Reset DataTable to default state |

### Bootloader Commands

| Num | Name | Description |
|-----|------|-------------|
| 320 | ACT_BOOT_LOADER_REQUEST | Restart processor for reprogramming |

### Debug Commands

| Num | Name | Description |
|-----|------|-------------|
| 50 | ACT_DBG_POWER_SUPPLY_CTRL | Test SCRelayBoard operation |
| 51 | ACT_DBG_PULSE | Generate control pulse on transistor gates |
| 52 | ACT_DBG_SET_CURRENT_RANGE | Set current range (0, 1) |
| 53 | ACT_DBG_FAN_CTRL | Test fan operation |
| 54 | ACT_DBG_IND_CTRL | Test external indicator operation |

### Flash Diagnostic Commands

| Num | Name | Description |
|-----|------|-------------|
| 330 | ACT_FLASH_DIAG_READ_SYMBOL | Read symbol from debug information memory |
| 331 | ACT_FLASH_DIAG_INIT_READ | Initialize debug information reading start |
| 332 | ACT_FLASH_DIAG_SAVE | Save debug information block to flash |
| 333 | ACT_FLASH_DIAG_ERASE | Erase debug information area |
| 340 | ACT_FLASH_DIAG_TO_EP | Read array from debug information memory to EP |
| 341 | ACT_JSON_INIT_READ | Initialize JSON reading start |
| 342 | ACT_JSON_TO_EP | Read JSON template to EP |

---

## 4. Endpoints

### Read-Only Endpoints (Float Type)

| Num | Name | Description | Data Type | Size |
|-----|------|-------------|-----------|------|
| 1 | EP_CURRENT | Current values array | float | 600 |
| 2 | EP_BATTERY_VOLTAGE | Battery voltage values array | float | 600 |
| 3 | EP_REGULATOR_OUTPUT | Regulator output values array | float | 600 |
| 4 | EP_REGULATOR_ERR | Regulator error values array | float | 600 |
| 5 | EP_CUR_TABLE | Current table values array | float | 600 |
| 6 | EP_DAC_RAW_DATA | DAC raw data values array | float | 600 |
| 20 | EP_ExtInfoData | External information from flash | float | 300 |

---

## Notes

### Data Table Structure
- **Non-Volatile Area**: Registers 0-127 (saved to flash)
- **Read/Write Area**: Registers 128-191 (volatile)
- **Read-Only Area**: Registers 192-255 (volatile)
- **Firmware Info Area**: Registers 256-300 (read-only)

### Register Categories
1. **Device Configuration**: Core device parameters and thresholds
2. **Calibration Parameters**: DAC and ADC calibration coefficients
3. **Regulator Parameters**: Control system parameters
4. **Runtime Parameters**: Operational settings
5. **Status Registers**: Device state and measurement results
6. **Firmware Information**: Version and identification data

### Endpoint Characteristics
- All endpoints are read-only and contain float data
- Array sizes are calculated based on timing parameters
- EP_ExtInfoData is used for external data storage

### Command Categories
1. **Power Control**: Enable/disable power supply
2. **System Control**: Device state management
3. **Data Management**: Flash operations and defaults
4. **Debug Commands**: Testing and diagnostic functions
5. **Bootloader**: Firmware update operations

### Error Codes
- **ERR_NONE**: No error
- **ERR_CONFIGURATION_LOCKED**: Device protected from writing
- **ERR_OPERATION_BLOCKED**: Operation cannot be performed in current state
- **ERR_DEVICE_NOT_READY**: Device not ready for state change
- **ERR_WRONG_PWD**: Wrong password

### Device States
- **DS_None**: Initial state
- **DS_Fault**: Fault condition
- **DS_Disabled**: Disabled state
- **DS_Ready**: Ready for operation
- **DS_ConfigReady**: Ready for configuration
- **DS_InProcess**: Processing operation

### Sub-States
- **SS_None**: No sub-state
- **SS_PowerPrepare**: Preparing power supply
- **SS_PulsePrepare**: Preparing pulse
- **SS_WaitAfterPulse**: Waiting after pulse
- **SS_Pulse**: Pulse execution 