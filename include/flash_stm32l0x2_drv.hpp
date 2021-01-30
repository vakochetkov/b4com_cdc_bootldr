/*
 * flash_stm32l0x2_drv.hpp
 * Driver provides unified interface for:
 *  NVM program memory
 *  EEPROM data memory
 *  OPTION bytes (TODO)
 *
 *  Created on: 13 янв. 2021 г.
 *      Author: Admin
 */

#ifndef INCLUDE_FLASH_STM32L0_DRV_HPP_
#define INCLUDE_FLASH_STM32L0_DRV_HPP_

#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "timeout.hpp"

#include "flash_drv.hpp"

#define __RAM_FUNC __attribute__ ((section(".ram")))

constexpr uint32_t FLASH_BASE_ADDR   = 0x08000000;
constexpr uint32_t EEPROM_BASE_ADDR  = 0x08080000;

/*
 * To perform a write/erase operation, unlock PELOCK bit of the FLASH_PECR register.
 * When this bit is unlocked (its value is 0), the other bits of the same register can be modified.
 * When PELOCK is 0, the write/erase operations can be executed in the DATA EEPROM.
 * To write/erase the Flash PROGRAM MEMORY, unlock PRGLOCK bit of the FLASH_PECR register.
 * The bit can only be unlocked when PELOCK is 0.
 * To write/erase the user OPTION BYTES, unlock OPTLOCK bit of the FLASH_PECR register.
 * The bit can only be unlocked when PELOCK is 0.
 * No relation exists between PRGLOCK and OPTLOCK: the first one can be unlocked when the second one is locked and vice versa
 */
template<uint32_t TBlockSizeNVM, uint32_t TBlockSizeEEPROM>
class flash_stm32l0x2_drv_c : public flash_drv_c {
	typedef union {
		struct {
			uint32_t PELOCK  : 1;
			uint32_t PRGLOCK : 1;
			uint32_t OPTLOCK : 1;
			uint32_t PROG    : 1;
			uint32_t DATA    : 1;
			uint32_t : 3;
			uint32_t FIX   : 1;
			uint32_t ERASE : 1;
			uint32_t FPRG  : 1;
			uint32_t : 4;
			uint32_t PARALELBANK : 1;
			uint32_t EOPIE : 1;
			uint32_t ERRIE : 1;
			uint32_t OBLLAUNCH : 1;
			uint32_t : 4;
			uint32_t NZDISABLE : 1;
			uint32_t : 8;
		} __attribute__((packed)) bit;
		uint32_t value;
	} FLASH_PECR_t;

	typedef union {
		struct {
			uint32_t LATENCY  : 1;
			uint32_t PRFTEN   : 1;
			uint32_t : 1;
			uint32_t SLEEPPD  : 1;
			uint32_t RUNPD    : 1;
			uint32_t DISABBUF : 1;
			uint32_t PREREAD  : 1;
			uint32_t : 25;
		} __attribute__((packed)) bit;
		uint32_t value;
	} FLASH_ACR_t;

	typedef union {
		struct {
			uint32_t BUSY   : 1;
			uint32_t EOP    : 1;
			uint32_t ENDHV  : 1;
			uint32_t READY  : 1;
			uint32_t : 4;
			uint32_t WRP_ERR  : 1;
			uint32_t PGA_ERR  : 1;
			uint32_t SIZE_ERR : 1;
			uint32_t OPTV_ERR : 1;
			uint32_t : 1;
			uint32_t RD_ERR   : 1;
			uint32_t : 2;
			uint32_t NOTZERO_ERR : 1;
			uint32_t FWW_ERR	   	: 1;
			uint32_t : 14;
		} __attribute__((packed)) bit;
		uint32_t value;
	} FLASH_SR_t;

	typedef struct
	{
		__IO FLASH_ACR_t  ACR;       /*!< Access control register,                     Address offset: 0x00 */
		__IO FLASH_PECR_t PECR;      /*!< Program/erase control register,              Address offset: 0x04 */
		__IO uint32_t PDKEYR;        /*!< Power down key register,                     Address offset: 0x08 */
		__IO uint32_t PEKEYR;        /*!< Program/erase key register,                  Address offset: 0x0c */
		__IO uint32_t PRGKEYR;       /*!< Program memory key register,                 Address offset: 0x10 */
		__IO uint32_t OPTKEYR;       /*!< Option byte key register,                    Address offset: 0x14 */
		__IO FLASH_SR_t SR;          /*!< Status register,                             Address offset: 0x18 */
		__IO uint32_t OPTR;          /*!< Option byte register,                        Address offset: 0x1c */
		__IO uint32_t WRPR;          /*!< Write protection register,                   Address offset: 0x20 */
	} FLASH_REG_TypeDef;

#define FLASHx ((FLASH_REG_TypeDef *) FLASH_R_BASE)

	const uint32_t EEPROM_LOCK_KEY1 = 0x89ABCDEF;
	const uint32_t EEPROM_LOCK_KEY2 = 0x02030405;

	const uint32_t NVM_LOCK_KEY1 = 0x8C9DAEBF;
	const uint32_t NVM_LOCK_KEY2 = 0x13141516;

	const uint32_t OPTBYTE_LOCK_KEY1 = 0xFBEAD9C8;
	const uint32_t OPTBYTE_LOCK_KEY2 = 0x24252627;

	uint32_t timeout = 100; // ms

	inline uint8_t isFlashBusy() noexcept {
		return FLASHx->SR.bit.BUSY;
	}

	inline uint8_t waitFlashBusy() noexcept {
		Timeout t;

		t.Set(timeout);
		while (isFlashBusy() && !t.IsTimeOut()) {
			t.Update();
		}

		if (t.IsTimeOut()) {
			return 0;
		} else {
			return 1;
		}
	}

	__RAM_FUNC void writeHalfPage(uint32_t addr) noexcept {
		// check address, then follow RM appendix
	}

	__RAM_FUNC void erasePage(uint32_t addr) noexcept {

	}

protected:
	is_supported_t is_supported = {
		1, // NVM_READ
		1, // NVM_WRITE
		1, // NVM_ERASE
		1, // EEPROM_READ
		1, // EEPROM_WRITE
		1, // EEPROM_ERASE
		0, // OPTBYTE_READ
		0  // OPTBYTE_MODIFY
	};

public:
	flash_stm32l0x2_drv_c() noexcept {

	}
	~flash_stm32l0x2_drv_c() noexcept {

	}

	void Init() noexcept {

	}

	inline uint32_t GetTimeout() noexcept {
		return timeout;
	}

	inline void SetTimeout(uint32_t ms) noexcept {
		timeout = ms;
	}

	constexpr uint32_t GetBlockSizeNVM() noexcept {
		return TBlockSizeNVM;
	}

	constexpr uint32_t GetBlockSizeEEPROM() noexcept {
		return TBlockSizeEEPROM;
	}

	// First key:  0x8C9DAEBF
	// Second key: 0x13141516
	uint8_t UnlockNVM() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}

		if (FLASHx->PECR.bit.PELOCK == 0) {
			if (FLASHx->PECR.bit.PGRLOCK != 0) {
				FLASH->PRGKEYR = NVM_LOCK_KEY1;
				FLASH->PRGKEYR = NVM_LOCK_KEY2;
			}
		}

		if (FLASHx->PECR.bit.PRGLOCK == 0) {
			return 1;
		} else {
			return 0;
		}
	}

	uint8_t LockNVM() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}
		FLASHx->PECR.bit.PRGLOCK = 1;
		return 1;
	}

	// First key:  0x89ABCDEF
	// Second key: 0x02030405
	uint8_t UnlockEEPROM() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}

		if (FLASHx->PECR.bit.PELOCK != 0)
		{
			FLASH->PEKEYR = EEPROM_LOCK_KEY1;
			FLASH->PEKEYR = EEPROM_LOCK_KEY2;
		}

		if (FLASHx->PECR.bit.PELOCK == 0) {
			return 1;
		} else {
			return 0;
		}
	}

	uint8_t LockEEPROM() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}
		FLASHx->PECR.bit.PELOCK = 1;
		return 1;
	}

	// First key:  0xFBEAD9C8
	// Second key: 0x24252627
	uint8_t UnlockOPT() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}

		if (FLASHx->PECR.bit.PELOCK == 0) {
			if (FLASHx->PECR.bit.OPTLOCK != 0) {
				FLASH->OPTKEYR = OPTBYTE_LOCK_KEY1;
				FLASH->OPTKEYR = OPTBYTE_LOCK_KEY2;
			}
		}

		if (FLASHx->PECR.bit.OPTLOCK == 0) {
			return 1;
		} else {
			return 0;
		}
	}

	uint8_t LockOPT() noexcept {
		if (!waitFlashBusy()) {
			return 0;
		}
		FLASHx->PECR.bit.OPTLOCK = 1;
		return 1;
	}

	void ReadBlockNVM(uint32_t addr, uint32_t * buffer, uint32_t length) noexcept {
		if (length < TBlockSizeNVM) {
			return;
		}
		if ((length % sizeof(uint32_t)) != 0) {
			return;
		}

		for (uint32_t i = 0; i < (TBlockSizeNVM / sizeof(uint32_t)); i++) {
			buffer[i * sizeof(uint32_t)] = *(__IO uint32_t*)(addr + i * sizeof(uint32_t));
		}
	}

	void WriteBlockNVM(uint32_t addr, uint32_t * buffer, uint32_t length) noexcept {
		//writeHalfPage 1
		//writeHalfPage 2
	}

	void EraseBlockNVM(uint32_t addr) noexcept {
		// erasePage
	}

	void ReadBlockEEPROM() noexcept {

	}

	void WriteBlockEEPROM() noexcept {

	}

	void EraseBlockEEPROM() noexcept {

	}

	void ReadOptByte() noexcept {
		// isTODO:
	}

	void WriteOptByte() noexcept {
		// isTODO:
	}

	void EraseOptByte() noexcept {
		// isTODO:
	}
};

extern flash_stm32l0x2_drv_c<128,2048> flash_stm32l0x2_drv;

#undef FLASHx

#endif /* INCLUDE_FLASH_STM32L0_DRV_HPP_ */
