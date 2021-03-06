#include <stdlib.h>

#include "display.h"
#include "lr35902.h"
#include "memory.h"

void Display::update(uint cycles)
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);

  if (LCDC & (1<<7)) // LCD enabled?
  {
    scanline_counter -= cycles;

    if (scanline_counter <= 0)
    {
      scanline_counter += cycles_per_scanline;

      u8 scanline = memory.get8(Memory::IO::LY) + 1;
      if (scanline > 153)
      {
        scanline = 0;
        vblank = false;
      }
      memory.direct_io_write8(Memory::IO::LY, scanline);

      if (scanline < 144)
      {
        draw_scanline();
      }
      else if (scanline == 144)
      {
        cpu.raise_interrupt(LR35902::Interrupt::VBLANK);
        vblank = true;
      }
    }

    update_status(); // put at beginning?
  }
}

void Display::draw_scanline()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);

  if (gb_version == GB_VERSION::COLOUR || LCDC & (1<<0))
  {
    draw_background();
  }
  else
  {
    clear_background();
  }
  if (LCDC & (1<<5))
  {
    draw_window();
  }
  if (LCDC & (1<<1))
  {
    draw_sprites();
  }
}

void Display::draw_background()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);

  uint base_tile_map_addr;
  if (LCDC & (1<<3))
  {
    // 0x9C00 - 9FFF
    base_tile_map_addr = 0x9C00;
  }
  else
  {
    // 0x9800 - 9BFF
    base_tile_map_addr = 0x9800;
  }

  uint base_tile_data_addr;
  bool signed_pattern_numbers;
  if (LCDC & (1<<4))
  {
    // 0x8000 - 0x8FFF
    // Offsets will be unsigned
    base_tile_data_addr = 0x8000;
    signed_pattern_numbers = false;
  }
  else
  {
    // 0x8800 - 0x97FF
    // Offsets will be signed, set addr to middle of range
    base_tile_data_addr = 0x9000;
    signed_pattern_numbers = true;
  }

  u8 SCY = memory.get8(Memory::IO::SCY);
  u8 SCX = memory.get8(Memory::IO::SCX);

  uint background_y = (SCY + LY)%256;
  uint tile_row = background_y/8;
  uint tile_y = background_y%8;

  for (uint screenx=0; screenx<width; screenx++)
  {
    uint background_x = (SCX + screenx)%256;
    uint tile_col = background_x/8;
    uint tile_x = background_x%8;

    // Tile map is 32x32 tiles, with 1 byte per tile
    uint tile_map_addr = base_tile_map_addr + tile_row*32 + tile_col;
    u8 tile_num = memory.get8(tile_map_addr, 0);

    u8 tile_attr;
    uint vram_bank;
    bool high_priority;
    if (gb_version == GB_VERSION::ORIGINAL)
    {
      vram_bank = 0;
      high_priority = false;
    }
    else
    {
      // Tile map attributes defines attributes for the corresponding tile number
      tile_attr = memory.get8(tile_map_addr, 1);

      vram_bank     = (tile_attr >> 3) & 0x1;
      bool flip_x   = (tile_attr >> 5) & 0x1;
      bool flip_y   = (tile_attr >> 6) & 0x1;
      high_priority = (tile_attr >> 7) & 0x1;

      if (!(LCDC & (1<<0))) // master priority
      {
        high_priority = false;
      }
      if (flip_y)
      {
        tile_y = 8 - tile_y;
      }
      if (flip_x)
      {
        tile_x = 8 - tile_x;
      }
    }


    // Tile data is 32x32 tiles, with 16 bytes per tile
    uint tile_data_addr;
    if (signed_pattern_numbers)
    {
      tile_data_addr = base_tile_data_addr + ((s8)tile_num)*16;
    }
    else
    {
      tile_data_addr = base_tile_data_addr + ((u8)tile_num)*16;
    }

    // Tiles are 8x8 pixels, with 2 bits per pixel
    // i.e 2 bytes per line and 4 pixels per byte
    //   The 2 bits per pixel aren't adjacent, they are in the
    //   same position in each of the 2 bytes for their line.
    uint tile_byte_offset = tile_y*2;
    u8 tile_byte1 = memory.get8(tile_data_addr + tile_byte_offset, vram_bank);
    u8 tile_byte2 = memory.get8(tile_data_addr + tile_byte_offset + 1, vram_bank);

    uint bit = 7 - tile_x;
    uint colour_id = ((tile_byte1 >> bit) & 0x1) |
                     ((tile_byte2 >> bit) & 0x1) << 1;

    Colour colour;
    if (gb_version == GB_VERSION::ORIGINAL)
    {
      // Get the colour from the background palette register
      // 0 = white, 3 = black
      u8 BGP = memory.get8(Memory::IO::BGP);
      u8 palette_colour = (BGP >> (colour_id * 2)) & 0x3;
      colour = display_palette[palette_colour];
    }
    else
    {
      u8 palette_num   = (tile_attr >> 0) & 0x7;
      uint palette_offset = palette_num*8; // palettes are 8 bytes each

      Colour cgb_display_palette[4];
      for (uint i=0; i<4; i++)
      {
        u8 colour_byte1 = cgb_background_palettes.at(palette_offset + i*2);
        u8 colour_byte2 = cgb_background_palettes.at(palette_offset + i*2 + 1);
        u16 c = colour_byte2 << 8 | colour_byte1;

        cgb_display_palette[i] = {.r = (u8)((c >> 0) & 0x1f),
                                  .g = (u8)((c >> 5) & 0x1f),
                                  .b = (u8)((c >>10) & 0x1f)};
      }

      colour = cgb_display_palette[colour_id];
    }

    // TODO priority tiles
    framebuffer[LY][screenx] = colour;
  }
}

void Display::clear_background()
{
  u8 LY = memory.get8(Memory::IO::LY);
  for (uint screenx=0; screenx<width; screenx++)
  {
    framebuffer[LY][screenx] = display_palette[0]; // white
  }
}

void Display::draw_window()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);
  u8 WY   = memory.get8(Memory::IO::WY);
  u8 WX   = memory.get8(Memory::IO::WX);

  if (WY > LY || WX >= 167)
  {
    // Window isn't visible / not drawn on this scanline
    return;
  }

  uint base_window_map_addr;
  if (LCDC & (1<<6))
  {
    // 0x9C00 - 9FFF
    base_window_map_addr = 0x9C00;
  }
  else
  {
    // 0x9800 - 9BFF
    base_window_map_addr = 0x9800;
  }

  uint base_tile_data_addr;
  bool signed_pattern_numbers;
  if (LCDC & (1<<4))
  {
    // 0x8000 - 0x8FFF
    // Offsets will be unsigned
    base_tile_data_addr = 0x8000;
    signed_pattern_numbers = false;
  }
  else
  {
    // 0x8800 - 0x97FF
    // Offsets will be signed, set addr to middle of range
    base_tile_data_addr = 0x9000;
    signed_pattern_numbers = true;
  }

  uint window_y = (LY - WY)%256;
  uint tile_row = window_y/8;
  uint tile_y = window_y%8;

  // Set WX to left side of window
  WX -= 7;

  for (uint screenx=0; screenx<width; screenx++)
  {
    uint window_x = (screenx - WX)%256;
    uint tile_col = window_x/8;
    uint tile_x = window_x%8;

    // Tile map is 32x32 tiles, with 1 byte per tile
    uint tile_map_addr = base_window_map_addr + tile_row*32 + tile_col;
    u8 tile_num = memory.get8(tile_map_addr, 0);

    u8 tile_attr;
    uint vram_bank;
    bool high_priority;
    if (gb_version == GB_VERSION::ORIGINAL)
    {
      vram_bank = 0;
      high_priority = false;
    }
    else
    {
      // Tile map attributes defines attributes for the corresponding tile number
      tile_attr = memory.get8(tile_map_addr, 1);

      vram_bank     = (tile_attr >> 3) & 0x1;
      bool flip_x   = (tile_attr >> 5) & 0x1;
      bool flip_y   = (tile_attr >> 6) & 0x1;
      high_priority = (tile_attr >> 7) & 0x1;

      if (!(LCDC & (1<<0))) // master priority
      {
        high_priority = false;
      }
      if (flip_y)
      {
        tile_y = 8 - tile_y;
      }
      if (flip_x)
      {
        tile_x = 8 - tile_x;
      }
    }

    // Tile data is 32x32 tiles, with 16 bytes per tile
    uint tile_data_addr;
    if (signed_pattern_numbers)
    {
      tile_data_addr = base_tile_data_addr + ((s8)tile_num)*16;
    }
    else
    {
      tile_data_addr = base_tile_data_addr + ((u8)tile_num)*16;
    }

    // Tiles are 8x8 pixels, with 2 bits per pixel
    // i.e 2 bytes per line and 4 pixels per byte
    //   The 2 bits per pixel aren't adjacent, they are in the
    //   same position in each of the 2 bytes for their line.
    uint tile_byte_offset = tile_y*2;
    u8 tile_byte1 = memory.get8(tile_data_addr + tile_byte_offset, vram_bank);
    u8 tile_byte2 = memory.get8(tile_data_addr + tile_byte_offset + 1, vram_bank);

    uint bit = 7 - tile_x;
    uint colour_id = ((tile_byte1 >> bit) & 0x1) |
                     ((tile_byte2 >> bit) & 0x1) << 1;

    Colour colour;
    if (gb_version == GB_VERSION::ORIGINAL)
    {
      // Get the colour from the background palette register
      // 0 = white, 3 = black
      u8 BGP = memory.get8(Memory::IO::BGP);
      u8 palette_colour = (BGP >> (colour_id * 2)) & 0x3;
      colour = display_palette[palette_colour];
    }
    else
    {
      u8 palette_num   = (tile_attr >> 0) & 0x7;
      uint palette_offset = palette_num*8; // palettes are 8 bytes each

      Colour cgb_display_palette[4];
      for (uint i=0; i<4; i++)
      {
        u8 colour_byte1 = cgb_background_palettes.at(palette_offset + i*2);
        u8 colour_byte2 = cgb_background_palettes.at(palette_offset + i*2 + 1);
        u16 c = colour_byte2 << 8 | colour_byte1;

        cgb_display_palette[i] = {.r = (u8)((c >> 0) & 0x1f),
                                  .g = (u8)((c >> 5) & 0x1f),
                                  .b = (u8)((c >>10) & 0x1f)};
      }

      colour = cgb_display_palette[colour_id];
    }

    // TODO priority tiles
    framebuffer[LY][screenx] = colour;
  }
}

void Display::draw_sprites()
{
  u8 LCDC = memory.get8(Memory::IO::LCDC);
  u8 LY   = memory.get8(Memory::IO::LY);

  const uint base_sprite_data_addr = 0x8000;
  const uint base_sprite_attr_addr = 0xfe00;

  bool use_8x16_sprites = (LCDC >> 2) & 0x1;
  uint sprite_height = use_8x16_sprites ? 16 : 8;

  // Go through all the sprites in reverse order
  // so sprite 0 is drawn on top of sprite 39 etc.
  for (int i=39; i>=0; i--)
  {
    // Sprite attribute blocks are 4 bytes each
    uint sprite_attr_addr = base_sprite_attr_addr + i*4;

    u8 y_pos = memory.get8(sprite_attr_addr);     // sprite top y coordinate + 16
    u8 x_pos = memory.get8(sprite_attr_addr + 1); // sprite left x coordinate + 8
    u8 pattern_number = memory.get8(sprite_attr_addr + 2);
    u8 flags = memory.get8(sprite_attr_addr + 3);

    if (y_pos > LY + 16 || y_pos + sprite_height <= (uint)(LY + 16))
    {
      // This sprite doesn't appear on the current scanline
      continue;
    }

    bool low_priority = (flags >> 7) & 0x1;
    bool flip_y = (flags >> 6) & 0x1;
    bool flip_x = (flags >> 5) & 0x1;
    uint palette_num = (flags >> 4) & 0x1;

    if (gb_version == GB_VERSION::COLOUR && !(LCDC & (1<<0))) // master priority
    {
      low_priority = false;
    }

    // Sprites are either 8x8 or 8x16 pixels, with eiter 16 or 32 bytes each.
    // 8x16 sprites are restricted to only even pattern numbers, so we can
    // just treat all patterns as 16 bytes wide
    if (use_8x16_sprites)
    {
      pattern_number &= 0xfe; // Set LSB to 0
    }
    uint sprite_data_addr = base_sprite_data_addr + pattern_number*0x10;

    uint sprite_y = LY - y_pos + 16;
    if (flip_y)
    {
      sprite_y = sprite_height - sprite_y;
    }

    uint vram_bank;
    if (gb_version == GB_VERSION::ORIGINAL)
    {
      vram_bank = 0;
    }
    else
    {
      vram_bank = (flags >> 3) & 0x1;
    }
    // Get the data for this line of the sprite
    uint sprite_byte_offset = sprite_y*2;
    u8 sprite_byte1 = memory.get8(sprite_data_addr + sprite_byte_offset, vram_bank);
    u8 sprite_byte2 = memory.get8(sprite_data_addr + sprite_byte_offset + 1, vram_bank);

    for (uint sprite_x=0; sprite_x<8; sprite_x++)
    {
      uint screenx = (x_pos-8+sprite_x)%256;
      if (screenx >= width)
      {
        // Pixel is off screen
        continue;
      }

      uint bit = flip_x ? sprite_x : 7 - sprite_x;
      uint colour_id = ((sprite_byte1 >> bit) & 0x1) |
                       ((sprite_byte2 >> bit) & 0x1) << 1;

      if (colour_id == 0) // Colour 0 is transparent
      {
        continue;
      }

      Colour colour;
      Colour colour_0;
      if (gb_version == GB_VERSION::ORIGINAL)
      {
        // Get the colour from the background palette register
        // 0 = white, 3 = black
        u8 palette;
        if (palette_num == 0)
        {
          palette = memory.get8(Memory::IO::OBP0);
        }
        else
        {
          palette = memory.get8(Memory::IO::OBP1);
        }
        u8 palette_colour = (palette >> (colour_id * 2)) & 0x3;
        colour = display_palette[palette_colour];

        u8 BGP = memory.get8(Memory::IO::BGP);
        u8 palette_colour_0 = (BGP & 0x3);
        colour_0 = display_palette[palette_colour_0];
      }
      else
      {
        palette_num = flags & 0x7;
        uint palette_offset = palette_num*8; // palettes are 8 bytes each

        Colour cgb_display_palette[4];
        for (uint i=0; i<4; i++)
        {
          u8 colour_byte1 = cgb_sprite_palettes.at(palette_offset + i*2);
          u8 colour_byte2 = cgb_sprite_palettes.at(palette_offset + i*2 + 1);
          u16 c = colour_byte2 << 8 | colour_byte1;

          cgb_display_palette[i] = {.r = (u8)((c >> 0) & 0x1f),
                                    .g = (u8)((c >> 5) & 0x1f),
                                    .b = (u8)((c >>10) & 0x1f)};
        }

        colour_0 = cgb_display_palette[0];
        colour = cgb_display_palette[colour_id];
      }

      // Low priority sprites are only drawn on colour 0 backgrounds
      if (!low_priority ||
          (framebuffer[LY][screenx].r == colour_0.r &&
           framebuffer[LY][screenx].b == colour_0.b &&
           framebuffer[LY][screenx].g == colour_0.g))
      {
        framebuffer[LY][screenx] = colour;
      }
    }
  }
}

void Display::update_status()
{
  u8 STAT = memory.get8(Memory::IO::STAT);

  u8 scanline = memory.get8(Memory::IO::LY);
  MODE::Mode mode;

  if (scanline > 144)
  {
    mode = MODE::VBLANK;
  }
  else if (scanline_counter > cycles_per_scanline - 80)
  {
    mode = MODE::OAM;
  }
  else if (scanline_counter > cycles_per_scanline - 80 - 172)
  {
    mode = MODE::VRAM;
  }
  else
  {
    mode = MODE::HBLANK;
  }

  const MODE::Mode current_mode = static_cast<MODE::Mode>(STAT & 0x3);
  if (current_mode != mode &&
      mode != MODE::VRAM &&
      (STAT >> (mode + 3)) & 0x1)
  {
    cpu.raise_interrupt(LR35902::Interrupt::LCD);
  }

  // Update mode
  STAT &= ~0x3;
  STAT |= mode;

  const u8 LYC = memory.get8(Memory::IO::LYC);
  if (LYC == scanline)
  {
    // Set coincidence flag
    STAT |= (1<<2);
    if ((STAT >> 6) & 0x1)
    {
      cpu.raise_interrupt(LR35902::Interrupt::LCD);
    }
  }
  else
  {
    // Clear coincidence flag
    STAT &= ~(1<<2);
  }

  memory.set8(Memory::IO::STAT, STAT);
}

u8 Display::read_byte(uint address) const
{
  switch (address)
  {
    case Memory::IO::BGPD:
      return cgb_background_palettes[cgb_background_palette_index];
    case Memory::IO::OBPD:
      return cgb_sprite_palettes[cgb_sprite_palette_index];
    case Memory::IO::BGPI:
      return (cgb_background_palette_autoinc << 7) | cgb_background_palette_index;
    case Memory::IO::OBPI:
      return (cgb_sprite_palette_autoinc << 7) | cgb_sprite_palette_index;
    default:
      abort();
  }
}

void Display::write_byte(uint address, u8 value)
{
  switch (address)
  {
    case Memory::IO::BGPD:
      cgb_background_palettes[cgb_background_palette_index] = value;
      if (cgb_background_palette_autoinc)
        cgb_background_palette_index++;
      break;
    case Memory::IO::OBPD:
      cgb_sprite_palettes[cgb_sprite_palette_index] = value;
      if (cgb_sprite_palette_autoinc)
        cgb_sprite_palette_index++;
      break;
    case Memory::IO::BGPI:
      cgb_background_palette_index = value & 0x3f;
      cgb_background_palette_autoinc = (value >> 7) & 0x1;
      break;
    case Memory::IO::OBPI:
      cgb_sprite_palette_index = value & 0x3f;
      cgb_sprite_palette_autoinc = (value >> 7) & 0x1;
      break;
    default:
      abort();
  }
}
