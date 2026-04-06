#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Включение блока
#define ACT_DISABLE_POWER				2	// Выключение блока
#define ACT_CLR_FAULT					3	// Очистка всех fault
#define ACT_CLR_WARNING					4	// Очистка всех warning

#define ACT_DBG_POWER_SUPPLY_CTRL		50	// Проверка работы SCRelayBoard
//
#define ACT_DBG_SET_CURRENT_RANGE		52	// Установка диапазона по току (0, 1)
#define ACT_DBG_FAN_CTRL				53	// Проверка работы вентилятора
#define ACT_DBG_IND_CTRL				54	// Проверка работы внешнего индикатора

#define ACT_CONFIG_UNIT					100	// Конфигурация блока
#define ACT_SOFTWARE_START				101	// Старт измерения
#define ACT_STOP_PROCESS				102	// Стоп измерения

#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования

#define ACT_FLASH_DIAG_READ_SYMBOL		330	// Выполнить чтение символа из памяти отладочной информации
#define ACT_FLASH_DIAG_INIT_READ		331	// Инициализировать начало считывания отладочной информации

#define ACT_FLASH_DIAG_SAVE				332	// Сохранение блока отладочной информации во флэш
#define ACT_FLASH_DIAG_ERASE			333	// Стирание области отладочной информации

#define ACT_FLASH_DIAG_TO_EP			340	// Выполнить чтение массива из памяти отладочной информации в EP
#define ACT_JSON_INIT_READ				341	// Инициализация начала считывания JSON
#define ACT_JSON_TO_EP					342	// Выполнить чтение шаблона JSON в EP

// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_CURRENT_THRESHOLD_HIGH		0	// Значение тока переключения диапазонов (между 1 и 2 диапазонами) (А)
#define REG_BATTERY_VOLTAGE_THRESHOLD	1	// Порог заряда батареи конденсаторов (В)
#define REG_DAC_OUTPUT_LIMIT_VALUE		2	// Ограничение выхода ЦАП (0 - 4095)
#define REG_BATTERY_FULL_CHRAGE_TIMEOUT	3	// Время таймаута полного заряда конденсаторов (мс)
#define REG_BATTERY_RECHARGE_TIMEOUT	4	// Время таймаута дозаряда после импульса (мс)
#define REG_SHUNT_RESISTANCE			5	// Сопротивление шунта (мкОм)
#define REG_REGULATOR_QI_MAX			6	// Ограничение уровня интегральной составляющей
#define REG_FAN_CTRL					7	// Включение управления вентилятором
#define REG_EXT_IND_CTRL				8	// Включение управления внешним индикатором
#define REG_AFTER_PULSE_PAUSE			9	// Пауза после импульса (мс)
#define REG_CURRENT_PER_CURBOARD		10	// Максимальная амплитуда тока с одной CurrentBoard (А)
#define REG_CURBOARDS					11	// Количество CurrentBoard
// 12
#define REG_FAN_OPERATE_PERIOD			13	// Период работы вентилятора (сек)
#define REG_FAN_OPERATE_TIME			14	// Время включенного состояния вентилятора (сек)
// 15
#define REG_CONFIG_READY_STATE_TIMEOUT	16	// Таймаут нахождения блока в состоянии DS_ConfigReady
#define REG_TRAPEZE_CURRENT_RATE		17	// dI/dt фронтов трапецеидального импульса тока (А/мкс)
// 18
#define REG_PULSE_SHAPE					19	// Форма импульса (0 - полусинус, 1 - мод. полусинус, 2 - трапеция)
//
#define REG_DAC_I_RANGE0_P2				20	// Диапазон 0 - Коэффициент тонкой подстройки Р2
#define REG_DAC_I_RANGE0_P1				21	// Диапазон 0 - Коэффициент тонкой подстройки Р1
#define REG_DAC_I_RANGE0_P0				22	// Диапазон 0 - Коэффициент тонкой подстройки Р0
#define REG_DAC_I_RANGE0_K				23	// Диапазон 0 - Коэффициент пересчета в значение ЦАП
#define REG_DAC_I_RANGE0_B				24	// Диапазон 0 - Смещение пересчета в значение ЦАП
//
#define REG_DAC_I_RANGE1_P2				25	// Диапазон 1 - Коэффициент тонкой подстройки Р2
#define REG_DAC_I_RANGE1_P1				26	// Диапазон 1 - Коэффициент тонкой подстройки Р1
#define REG_DAC_I_RANGE1_P0				27	// Диапазон 1 - Коэффициент тонкой подстройки Р0
#define REG_DAC_I_RANGE1_K				28	// Диапазон 1 - Коэффициент пересчета в значение ЦАП
#define REG_DAC_I_RANGE1_B				29	// Диапазон 1 - Смещение пересчета в значение ЦАП
//
#define REG_ADC_VOLTAGE_K				30	// Измерение напряжения на батарее, пропорциональный коэффициент K
#define REG_ADC_VOLTAGE_B				31	// Измерение напряжения на батарее, коэффициент cмещения B
//
#define REG_K_AMP_RANGE0				32	// Коэффициент усиления тока в канале 0
#define REG_K_AMP_RANGE1				33	// Коэффициент усиления тока в канале 1
//
#define REG_ADC_I_RANGE0_1_P2			34	// Диапазон 0 - Коэффициент тонкой подстройки Р2
#define REG_ADC_I_RANGE0_1_P1			35	// Диапазон 0 - Коэффициент тонкой подстройки Р1
#define REG_ADC_I_RANGE0_1_P0			36	// Диапазон 0 - Смещение тонкой подстройки Р0
#define REG_ADC_I_RANGE0_1_K			37	// Диапазон 0 - Коэффициент грубой подстройки K
#define REG_ADC_I_RANGE0_1_B			38	// Диапазон 0 - Смещение грубой подстройки B
//
#define REG_ADC_I_RANGE1_P2				39	// Диапазон 1 и 2 - Коэффициент тонкой подстройки Р2
#define REG_ADC_I_RANGE1_P1				40	// Диапазон 1 и 2 - Коэффициент тонкой подстройки Р1
#define REG_ADC_I_RANGE1_P0				41	// Диапазон 1 и 2 - Смещение тонкой подстройки Р0
#define REG_ADC_I_RANGE1_K				42	// Диапазон 1 и 2 - Коэффициент грубой подстройки K
#define REG_ADC_I_RANGE1_B				43	// Диапазон 1 и 2 - Смещение тонкой подстройки B
//
#define REG_REGULATOR_RANGE0_Kp			44	// Диапазон 0 - Пропорциональный коэффициент регулятора
#define REG_REGULATOR_RANGE0_Ki			45	// Диапазон 0 - Интегральный коэффициент регулятора
#define REG_REGULATOR_RANGE1_Kp			46	// Диапазон 1 - Пропорциональный коэффициент регулятора
#define REG_REGULATOR_RANGE1_Ki			47	// Диапазон 1 - Интегральный коэффициент регулятора
// 48 - 50
#define REG_REGULATOR_ALLOWED_ERR		51	// Максимально допустимая ошибка регулятора
#define REG_FOLLOWING_ERR_CNT			52	// Максимальное значение счетчика FollowingError
#define REG_FOLLOWING_ERR_MUTE			53	// Выключение слежения за FollowingError
//
#define REG_REF_VOLTAGE_VARIABLE		54	// Переменное значение опорного напряжения на платах версии 1.1
// 55 - 59
#define REG_CFG_NODE_ID					60	// Настройка CAN NodeID
#define REG_LOW_ITM_LIMIT				61	// Минимальное допустимое значение по Itm
#define REG_HIGH_ITM_LIMIT				62	// Максимальное допустимое значение по Itm
//
#define REG_CURRENT_THRESHOLD_LOW		63	// Значение тока переключения диапазонов (между 0 и 1 диапазонами) (А)
//
#define REG_DAC_I_RANGE2_P2				64	// Диапазон 2 - Коэффициент тонкой подстройки Р2
#define REG_DAC_I_RANGE2_P1				65	// Диапазон 2 - Коэффициент тонкой подстройки Р1
#define REG_DAC_I_RANGE2_P0				66	// Диапазон 2 - Коэффициент тонкой подстройки Р0
#define REG_DAC_I_RANGE2_K				67	// Диапазон 2 - Коэффициент пересчета в значение ЦАП
#define REG_DAC_I_RANGE2_B				68	// Диапазон 2 - Смещение пересчета в значение ЦАП
// 69
#define REG_REGULATOR_RANGE2_Kp			70	// Диапазон 2 - Пропорциональный коэффициент регулятора
#define REG_REGULATOR_RANGE2_Ki			71	// Диапазон 2 - Интегральный коэффициент регулятора
//
#define REG_PCB_VERSION					120	// 0 - версия платы 1.0
											// 1 - версия  1.1
// Несохраняемые регистры чтения-записи
#define REG_CURRENT_PULSE_VALUE			128	// Задание амплитуды импульса тока (А)
#define REG_TRAPEZE_FLATTOP				129	// Длительность полки тока трапеции (мс)
//
#define REG_DBG							150	// Регистр режима Отладки
#define REG_DBG_DISABLE_REGLTR			151	// Отключение коэффициентов регулятора
#define REG_DBG_DISABLE_DAC_OUTPUT		152	// Отключение выхода ЦАП

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_SUB_STATE					198	// Регистр вспомогательного состояния

#define REG_RESULT_CURRENT				200	// Достигнутое значение импульса тока (А)
#define REG_BATTERY_VOLTAGE				201	// Напряжение на батарее конденсаторов (В)
#define REG_RESULT_MAX_DAC				202	// Значение ЦАП в точке измерения тока
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
#define DF_PROBLEM_BATTERY				1

// Problem
#define PROBLEM_NONE					0
#define PROBLEM_FOLLOWING_ERROR			1
#define PROBLEM_SYNC_STOP				2
#define PROBLEM_MANUAL_STOP				3
#define PROBLEM_TRAPEZE_INDEX			4

//  Warning
#define WARNING_NONE					0

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	//  Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	//  Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	//  Неправильный ключ

// Endpoints
#define EP_CURRENT						1
#define	EP_BATTERY_VOLTAGE				2
#define EP_REGULATOR_OUTPUT				3
#define EP_REGULATOR_ERR				4
#define EP_CUR_TABLE					5
#define EP_DAC_RAW_DATA					6

#define EP_ExtInfoData					20	// External information from flash

#endif //  __DEV_OBJ_DIC_H
