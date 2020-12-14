/*
 * gpio.hpp
 *
 *  Created on: 23 нояб. 2020 г.
 *      Author: kochetkov
 *
 * Настройка GPIO:
 * 1. Инициализировать (1 раз на все пины)
 * 2. Обнулить значения пинов
 * 3. Сконфигурировать пин
 */

#ifndef INCLUDE_GPIO_HPP_
#define INCLUDE_GPIO_HPP_

#include"stm32l0xx_common.hpp"

class gpio_c {
public:
	enum class GPIO_PORT_t : uint32_t {
			PORTA = GPIOA_BASE,
			PORTB = GPIOB_BASE,
			PORTC = GPIOC_BASE
		};
		enum class GPIO_MODE_t : uint8_t {
			INPUT     = 0,
			OUTPUT    = 1,
			ALTERNATE = 2,
			ANALOG_IN = 3
		};
		enum class GPIO_SPEED_t : uint8_t {
			LOW       = 0b00,
			MEDIUM    = 0b01,
			HIGH 	  = 0b10,
			VERY_HIGH = 0b11
		};
		enum class GPIO_OUT_TYPE_t : uint8_t {
			OD = 1,
			PP = 0
		};
		enum class GPIO_PULL_TYPE_t : uint8_t {
			NO   = 0b00,
			UP   = 0b01,
			DOWN = 0b10
		};
		enum class GPIO_ALTERNATE_t : uint8_t {
			AF0 = 0b0000,
			AF1 = 0b0001,
			AF2 = 0b0010,
			AF3 = 0b0011,
			AF4 = 0b0100,
			AF5 = 0b0101,
			AF6 = 0b0110,
			AF7 = 0b0111
		};

		static constexpr volatile GPIO_TypeDef * PORTx(GPIO_PORT_t port) noexcept {
			return reinterpret_cast<volatile GPIO_TypeDef *>(port);
		}

		static void Init() noexcept {
			// Enable the peripheral clock of GPIOA and GPIOB
			RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
		}

		// Configuration
		static void ConfigAsOut(GPIO_PORT_t port, uint8_t pin, GPIO_SPEED_t speed,
				GPIO_OUT_TYPE_t otype, GPIO_PULL_TYPE_t ptype) noexcept {

			PORTx(port)->OSPEEDR |= (static_cast<uint8_t>(speed) << (pin * 2));
			PORTx(port)->OTYPER  |= (static_cast<uint8_t>(otype) << (pin));
			PORTx(port)->PUPDR   |= (static_cast<uint8_t>(ptype) << (pin * 2));
			PORTx(port)->MODER   |= (static_cast<uint8_t>(GPIO_MODE_t::OUTPUT) << (pin * 2));
		}

		static void ConfigAsInput(GPIO_PORT_t port, uint8_t pin, GPIO_PULL_TYPE_t ptype) noexcept {
			PORTx(port)->PUPDR   |= (static_cast<uint8_t>(ptype) << (pin * 2));
			PORTx(port)->MODER   |= (static_cast<uint8_t>(GPIO_MODE_t::INPUT) << (pin * 2));
		}

		static void ConfigAsAlternate(GPIO_PORT_t port, uint8_t pin, GPIO_SPEED_t speed,
									  GPIO_OUT_TYPE_t otype, GPIO_PULL_TYPE_t ptype, GPIO_ALTERNATE_t af) noexcept {

			PORTx(port)->OSPEEDR |= (static_cast<uint8_t>(speed) << (pin * 2));
			PORTx(port)->OTYPER  |= (static_cast<uint8_t>(otype) << (pin));
			PORTx(port)->PUPDR   |= (static_cast<uint8_t>(ptype) << (pin * 2));
			PORTx(port)->MODER   |= (static_cast<uint8_t>(GPIO_MODE_t::OUTPUT) << (pin * 2));
			PORTx(port)->AFR[((pin > 7) ? 1 : 0)] |= (static_cast<uint8_t>(af) << (pin * 4));
		}

		static void ConfigAsAnalog(GPIO_PORT_t port, uint8_t pin) noexcept {
			PORTx(port)->PUPDR   |= (static_cast<uint8_t>(GPIO_PULL_TYPE_t::NO) << (pin * 2));
			PORTx(port)->MODER   |= (static_cast<uint8_t>(GPIO_MODE_t::ANALOG_IN) << (pin * 2));
		}

		static void DeConfigure(GPIO_PORT_t port, uint8_t pin) noexcept {
			WritePin(port,pin,0);
			ConfigAsInput(port, pin, GPIO_PULL_TYPE_t::NO); // set default as input
		}

		// Write-Read
		static void WritePort(GPIO_PORT_t port, uint16_t value) noexcept {
			PORTx(port)->ODR = value;
		}
		static uint16_t ReadPort(GPIO_PORT_t port) noexcept {
			return static_cast<uint16_t>(PORTx(port)->IDR);
		}

		static void WritePin(GPIO_PORT_t port, uint8_t pin, uint8_t value) noexcept {
			PORTx(port)->BSRR |= ((value & 0x1) ? (1 << pin) : (1 << (pin + 16)));
		}
		static uint8_t ReadPin(GPIO_PORT_t port, uint8_t pin) noexcept {
			return ((static_cast<uint16_t>(PORTx(port)->IDR) >> pin) & 0x1);
		}
};

typedef gpio_c gpio;

#endif /* INCLUDE_GPIO_HPP_ */
