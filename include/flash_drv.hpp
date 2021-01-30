/*
 * flash_drv.hpp
 *
 *  Created on: 13 янв. 2021 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_FLASH_DRV_HPP_
#define INCLUDE_FLASH_DRV_HPP_

#include <cstdint>

/*
 * Interface declaration for all internal memory drivers
 */
class flash_drv_c {

protected:
	typedef struct { // XXX: possible unexpected struct alignment/padding
		uint8_t NVM_READ  : 1;
		uint8_t NVM_WRITE : 1;
		uint8_t NVM_ERASE : 1;
		uint8_t EEPROM_READ  : 1;
		uint8_t EEPROM_WRITE : 1;
		uint8_t EEPROM_ERASE : 1;
		uint8_t OPTBYTE_READ   : 1;
		uint8_t OPTBYTE_MODIFY : 1;
	} is_supported_t;

public:
	virtual ~flash_drv_c() noexcept {};

	virtual void Init() = 0;
	virtual uint32_t GetBlockSizeNVM() = 0;
	virtual uint32_t GetBlockSizeEEPROM() = 0;

	virtual uint8_t UnlockNVM() 	= 0;
	virtual uint8_t LockNVM() 		= 0;
	virtual uint8_t UnlockEEPROM()  = 0;
	virtual uint8_t LockEEPROM() 	= 0;
	virtual uint8_t UnlockOPT() 	= 0;
	virtual uint8_t LockOPT() 		= 0;

	virtual void ReadBlockNVM()	 = 0;
	virtual void WriteBlockNVM() = 0;
	virtual void EraseBlockNVM() = 0;

	virtual void ReadBlockEEPROM()	= 0;
	virtual void WriteBlockEEPROM() = 0;
	virtual void EraseBlockEEPROM() = 0;

	virtual void ReadOptByte()	= 0;
	virtual void WriteOptByte() = 0;
	virtual void EraseOptByte() = 0;
};



#endif /* INCLUDE_FLASH_DRV_HPP_ */
