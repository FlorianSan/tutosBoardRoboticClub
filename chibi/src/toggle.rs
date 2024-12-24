#![no_std]
#![no_main]
#![allow(unused)]

use core::panic::PanicInfo;
use core::ptr;

const GPIOA_BASE: u32 = 0x40020000;
const RCC_BASE: u32 = 0x40023800;

// Registres GPIOA
const GPIOA_MODER: u32 = GPIOA_BASE + 0x00; // Mode Register
const GPIOA_ODR: u32 = GPIOA_BASE + 0x14;   // Output Data Register
const GPIOA_BSRR: u32 = GPIOA_BASE + 0x18;  // Bit Set/Reset Register

// Registres RCC
const RCC_AHB1ENR: u32 = RCC_BASE + 0x30;   // AHB1 Peripheral Clock Enable Register

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[no_mangle]
pub extern "C" fn pin_cfg() {
    unsafe {
        // 1. Activation de l'horloge pour GPIOA (RCC_AHB1ENR)
        let rcc_ahb1enr = RCC_AHB1ENR as *mut u32;
        ptr::write_volatile(rcc_ahb1enr, 1 << 0); // Activer l'horloge pour GPIOA (bit 0)

        // 2. Configuration de PA5 comme sortie
        let moder = GPIOA_MODER as *mut u32;
        let current_moder = ptr::read_volatile(moder);
        // Configurer PA5 (bits 10 et 11 dans MODER) en mode sortie (0b01)
        ptr::write_volatile(moder, (current_moder & !(0b11 << 10)) | (0b01 << 10));
    }
}

#[no_mangle]
pub extern "C" fn pin_toggle() {
    unsafe {
        // Lire l'état actuel de PA5 (ODR)
        let odr = GPIOA_ODR as *mut u32;
        let current_state = ptr::read_volatile(odr);

        // Vérifier l'état de PA5 (bit 5 dans ODR)
        if (current_state & (1 << 5)) != 0 {
            // Si la broche est allumée, l'éteindre (RESET bit 5 dans BSRR)
            let bsrr = GPIOA_BSRR as *mut u32;
            ptr::write_volatile(bsrr, 1 << (5 + 16)); // Éteindre la broche (RESET bit 5)
        } else {
            // Sinon, l'allumer (SET bit 5 dans BSRR)
            let bsrr = GPIOA_BSRR as *mut u32;
            ptr::write_volatile(bsrr, 1 << 5); // Allumer la broche (SET bit 5)
        }
    }
}