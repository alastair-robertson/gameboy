#pragma once

#include <vector>
#include "types.h"

class Cartridge;
class Joypad;

class Memory
{
  Cartridge &cart;
  Joypad &joypad;

  std::vector<u8> vram = std::vector<u8>(0x2000);
  std::vector<u8> wram = std::vector<u8>(0x2000);
  std::vector<u8> hram = std::vector<u8>(0x7f);
  std::vector<u8> oam  = std::vector<u8>(0xa0);
  std::vector<u8> io   = std::vector<u8>(0x80);
  u8 interrupt_enable;

public:
  Memory() = delete;
  explicit Memory(Cartridge &cartridge, Joypad &j) : cart(cartridge),
                                                     joypad(j) { }

  void set8(uint address, u8 value)
  {
    write_byte(address, value);
  }

  u8 get8(uint address) const
  {
    return read_byte(address);
  }

  void set16(uint address, u16 value)
  {
    u8 upper = (value & 0xff00) >> 8;
    u8 lower = (value & 0x00ff);
    write_byte(address, lower);
    write_byte(address+1, upper);
  }

  u16 get16(uint address) const
  {
    u8 lower = read_byte(address);
    u8 upper = read_byte(address+1);
    u16 value = (upper << 8) | lower;
    return value;
  }

  // Used to directly set the memory at a given address
  // without value being manipulated first
  void direct_io_write8(uint address, u8 value);

  void print(uint start, uint range=10)
  {
    for (uint i=start; i<start+range; i++)
    {
//      printf("0x%04X: 0x%02X\n", i, mem8[i]);
    }
  }

  struct IO
  {
    enum Address
    {
      // Joypad
      JOYP = 0xff00,    // Joypad

      // Timer
      DIV  = 0xff04,    // Divider register
      TIMA = 0xff05,    // Timer counter
      TMA  = 0xff06,    // Timer Modulo
      TAC  = 0xff07,    // Timer Control

      // Display
      LCDC = 0xff40,    // LCD Control
      STAT = 0xff41,    // LCDC Status
      SCY  = 0xff42,    // Scroll Y
      SCX  = 0xff43,    // Scroll X
      LY   = 0xff44,    // LCDC Y coordinate (scanline)
      LYC  = 0xff45,    // LY Compare
      DMA  = 0xff46,    // Direct Memory Access
      BGP  = 0xff47,    // Background & Window Palette Data (non GBC)
      OBP0 = 0xff48,    // Object Palette 0 Data (non GBC)
      OBP1 = 0xff49,    // Object Palette 1 Data (non GBC)
      WY   = 0xff4a,    // Window Y position
      WX   = 0xff4b,    // Window X position

      // Interrupt
      IF = 0xff0f,      // Interrupt Flag
      IE = 0xffff,      // Interrupt Enable
    };
  };

private:
  u8 read_byte(uint address) const;
  void write_byte(uint address, u8 value);
  void dma_transfer(uint address);
};