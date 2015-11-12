#include <stdio.h>
#include <assert.h>

#include "LR35902.h"
#include "memory.h"

LR35902::InstrFunc LR35902::optable[LR35902::table_size];
LR35902::OpInfo LR35902::infotable[LR35902::table_size];
constexpr LR35902::OpInfo LR35902::unknown_info;
constexpr LR35902::Instruction LR35902::implemented_instruction_table[];

void LR35902::run()
{
  reg.pc = 0x100;
  reg.sp = 0xfffe;
  while (true)
  {
    uint8_t opcode = memory.get8(reg.pc);
    printf("%02X\n", opcode);
    execute(opcode);

    // TODO deal with interrupts here
  }
}

void LR35902::execute(uint8_t opcode)
{
  InstrFunc instr = optable[opcode];
  (this->*instr)();
  OpInfo info = infotable[opcode];
  reg.pc += info.length;
  // TODO count cycles here too
}

void LR35902::init_tables()
{
  static bool initialised = false;
  if (initialised)
    return;

  for (int i=0; i<table_size; i++)
  {
    optable[i] = &LR35902::unknown_instruction;
    infotable[i] = unknown_info;
  }

  for (const Instruction &instr : implemented_instruction_table)
  {
    assert(optable[instr.opcode] == &LR35902::unknown_instruction);

    optable[instr.opcode] = instr.func;
    infotable[instr.opcode] = instr.opinfo;
  }
}

void LR35902::unknown_instruction()
{
  fprintf(stderr, "Unknown instruction: %02X at %04X\n", memory.get8(reg.pc), reg.pc);
  abort();
}

void LR35902::NOP()
{
  // Do nothing
}

void LR35902::STOP()
{
  // Turn off screen and do nothing until button pressed
  // TODO
}

void LR35902::HALT()
{
  // Do nothing until interrupt occurs
  // TODO
}

void LR35902::DI()
{
  // TODO don't disable interrupts immediately
  interrupts_enabled = false;
}

void LR35902::EI()
{
  // TODO don't enable interrupts immediately
  interrupts_enabled = true;
}

void LR35902::LD_a_n()
{
  reg.a = memory.get8(reg.pc + 1);
}

void LR35902::LD_b_n()
{
  reg.b = memory.get8(reg.pc + 1);
}

void LR35902::LD_c_n()
{
  reg.c = memory.get8(reg.pc + 1);
}

void LR35902::LD_d_n()
{
  reg.d = memory.get8(reg.pc + 1);
}

void LR35902::LD_e_n()
{
  reg.e = memory.get8(reg.pc + 1);
}

void LR35902::LD_h_n()
{
  reg.h = memory.get8(reg.pc + 1);
}

void LR35902::LD_l_n()
{
  reg.l = memory.get8(reg.pc + 1);
}

void LR35902::LD_hl_n()
{
  u8 n = memory.get8(reg.pc + 1);
  memory.set8(reg.hl, n);
}

void LR35902::LD_a_a()
{
  reg.a = reg.a;
}

void LR35902::LD_a_b()
{
  reg.a = reg.b;
}

void LR35902::LD_a_c()
{
  reg.a = reg.c;
}

void LR35902::LD_a_d()
{
  reg.a = reg.d;
}

void LR35902::LD_a_e()
{
  reg.a = reg.e;
}

void LR35902::LD_a_h()
{
  reg.a = reg.h;
}

void LR35902::LD_a_l()
{
  reg.a = reg.l;
}

void LR35902::LD_a_hl()
{
  reg.a = memory.get8(reg.hl);
}

void LR35902::LD_b_a()
{
  reg.b = reg.a;
}

void LR35902::LD_b_b()
{
  reg.b = reg.b;
}

void LR35902::LD_b_c()
{
  reg.b = reg.c;
}

void LR35902::LD_b_d()
{
  reg.b = reg.d;
}

void LR35902::LD_b_e()
{
  reg.b = reg.e;
}

void LR35902::LD_b_h()
{
  reg.b = reg.h;
}

void LR35902::LD_b_l()
{
  reg.b = reg.l;
}

void LR35902::LD_b_hl()
{
  reg.b = memory.get8(reg.hl);
}

void LR35902::LD_c_a()
{
  reg.c = reg.a;
}

void LR35902::LD_c_b()
{
  reg.c = reg.b;
}

void LR35902::LD_c_c()
{
  reg.c = reg.c;
}

void LR35902::LD_c_d()
{
  reg.c = reg.d;
}

void LR35902::LD_c_e()
{
  reg.c = reg.e;
}

void LR35902::LD_c_h()
{
  reg.c = reg.h;
}

void LR35902::LD_c_l()
{
  reg.c = reg.l;
}

void LR35902::LD_c_hl()
{
  reg.c = memory.get8(reg.hl);
}

void LR35902::LD_d_a()
{
  reg.d = reg.a;
}

void LR35902::LD_d_b()
{
  reg.d = reg.b;
}

void LR35902::LD_d_c()
{
  reg.d = reg.c;
}

void LR35902::LD_d_d()
{
  reg.d = reg.d;
}

void LR35902::LD_d_e()
{
  reg.d = reg.e;
}

void LR35902::LD_d_h()
{
  reg.d = reg.h;
}

void LR35902::LD_d_l()
{
  reg.d = reg.l;
}

void LR35902::LD_d_hl()
{
  reg.d = memory.get8(reg.hl);
}

void LR35902::LD_e_a()
{
  reg.e = reg.a;
}

void LR35902::LD_e_b()
{
  reg.e = reg.b;
}

void LR35902::LD_e_c()
{
  reg.e = reg.c;
}

void LR35902::LD_e_d()
{
  reg.e = reg.d;
}

void LR35902::LD_e_e()
{
  reg.e = reg.e;
}

void LR35902::LD_e_h()
{
  reg.e = reg.h;
}

void LR35902::LD_e_l()
{
  reg.e = reg.l;
}

void LR35902::LD_e_hl()
{
  reg.e = memory.get8(reg.hl);
}

void LR35902::LD_h_a()
{
  reg.h = reg.a;
}

void LR35902::LD_h_b()
{
  reg.h = reg.b;
}

void LR35902::LD_h_c()
{
  reg.h = reg.c;
}

void LR35902::LD_h_d()
{
  reg.h = reg.d;
}

void LR35902::LD_h_e()
{
  reg.h = reg.e;
}

void LR35902::LD_h_h()
{
  reg.h = reg.h;
}

void LR35902::LD_h_l()
{
  reg.h = reg.l;
}

void LR35902::LD_h_hl()
{
  reg.h = memory.get8(reg.hl);
}

void LR35902::LD_l_a()
{
  reg.l = reg.a;
}

void LR35902::LD_l_b()
{
  reg.l = reg.b;
}

void LR35902::LD_l_c()
{
  reg.l = reg.c;
}

void LR35902::LD_l_d()
{
  reg.l = reg.d;
}

void LR35902::LD_l_e()
{
  reg.l = reg.e;
}

void LR35902::LD_l_h()
{
  reg.l = reg.h;
}

void LR35902::LD_l_l()
{
  reg.l = reg.l;
}

void LR35902::LD_l_hl()
{
  reg.l = memory.get8(reg.hl);
}

void LR35902::LD_hl_a()
{
  memory.set8(reg.hl, reg.a);
}

void LR35902::LD_hl_b()
{
  memory.set8(reg.hl, reg.b);
}

void LR35902::LD_hl_c()
{
  memory.set8(reg.hl, reg.c);
}

void LR35902::LD_hl_d()
{
  memory.set8(reg.hl, reg.d);
}

void LR35902::LD_hl_e()
{
  memory.set8(reg.hl, reg.e);
}

void LR35902::LD_hl_h()
{
  memory.set8(reg.hl, reg.h);
}

void LR35902::LD_hl_l()
{
  memory.set8(reg.hl, reg.l);
}

void LR35902::LD_bc_a()
{
  memory.set8(reg.bc, reg.a);
}

void LR35902::LD_de_a()
{
  memory.set8(reg.de, reg.a);
}

void LR35902::LD_hl_inc_a()
{
  memory.set8(reg.hl++, reg.a);
}

void LR35902::LD_hl_dec_a()
{
  memory.set8(reg.hl--, reg.a);
}

void LR35902::LD_a_bc()
{
  reg.a = memory.get8(reg.bc);
}

void LR35902::LD_a_de()
{
  reg.a = memory.get8(reg.de);
}

void LR35902::LD_a_hl_inc()
{
  reg.a = memory.get8(reg.hl++);
}

void LR35902::LD_a_hl_dec()
{
  reg.a = memory.get8(reg.hl--);
}

void LR35902::LDH_naddr_a()
{
  u8 n = memory.get8(reg.pc + 1);
  memory.set8(0xff00+n, reg.a);
}

void LR35902::LDH_a_naddr()
{
  u8 n = memory.get8(reg.pc + 1);
  reg.a = memory.get8(0xff00+n);
}

void LR35902::LD_caddr_a()
{
  memory.set8(0xff00+reg.c, reg.a);
}

void LR35902::LD_a_caddr()
{
  reg.a = memory.get8(0xff00+reg.c);
}

// TODO check endianess for 16-bit loads
void LR35902::LD_naddr_a()
{
  u16 n = memory.get16(reg.pc + 1);
  memory.set8(n, reg.a);
}

void LR35902::LD_a_naddr()
{
  u16 n = memory.get16(reg.pc + 1);
  reg.a = memory.get8(n);
}

void LR35902::LD_bc_d16()
{
  reg.bc = memory.get16(reg.pc + 1);
}

void LR35902::LD_de_d16()
{
  reg.de = memory.get16(reg.pc + 1);
}

void LR35902::LD_hl_d16()
{
  reg.hl = memory.get16(reg.pc + 1);
}

void LR35902::LD_sp_d16()
{
  reg.sp = memory.get16(reg.pc + 1);
}

void LR35902::LD_a16_sp()
{
  u16 n = memory.get16(reg.pc + 1);
  memory.set16(n, reg.sp);
}

void LR35902::POP_bc()
{
  reg.bc = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_de()
{
  reg.de = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_hl()
{
  reg.hl = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::POP_af()
{
  reg.af = memory.get16(reg.sp);
  reg.sp += 2;
}

void LR35902::PUSH_bc()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.bc);
}

void LR35902::PUSH_de()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.de);
}

void LR35902::PUSH_hl()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.hl);
}

void LR35902::PUSH_af()
{
  reg.sp -= 2;
  memory.set16(reg.sp, reg.af);
}

void LR35902::LD_hl_sp_r8()
{
  s8 n = memory.get8(reg.pc + 1);

  set_flag_z(false);
  set_flag_n(false);
  set_flag_h((reg.sp&0xf) + (n&0xf) > 0xf);
  set_flag_c(reg.sp + n > 0xff);

  reg.hl = reg.sp + n;
}

void LR35902::LD_sp_hl()
{
  reg.sp = reg.hl;
}

template <u8 LR35902::Reg::*R>
void LR35902::INC_R()
{
  set_flag_z(reg.*R + 1 == 0);
  set_flag_n(false);
  set_flag_h((reg.*R&0xf) + 1 > 0xf);

  (reg.*R)++;
}

void LR35902::INC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)+1);
}

template <u8 LR35902::Reg::*R>
void LR35902::DEC_R()
{
  set_flag_z(reg.*R - 1 == 0);
  set_flag_n(true);
  set_flag_h((reg.*R&0xf) >= 1);

  (reg.*R)--;
}

void LR35902::DEC_hladdr()
{
  memory.set8(reg.hl, memory.get8(reg.hl)-1);
}

void LR35902::DAA()
{
  // Decimal Adjust register A

  bool carry = false;

  u8 a = reg.a;
  if (get_flag_n())
  {
    // Decimal adjust after subtraction
    if ((reg.a&0xf) > 0x9 || get_flag_h())
    {
      a -= 0x6;
    }
    if (reg.a > 0x99 || get_flag_c())
    {
      a -= 0x60;
      carry = true;
    }
  }
  else
  {
    // Decimal adjust after addition
    if ((reg.a&0xf) > 0x9 || get_flag_h())
    {
      a += 0x6;
    }
    if (reg.a > 0x99 || get_flag_c())
    {
      a += 0x60;
      carry = true;
    }

  }
  reg.a = a;

  set_flag_z(reg.a == 0);
  set_flag_h(false);
  set_flag_c(carry);
}

void LR35902::SCF()
{
  // Set Carry Flag

  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(true);
}

void LR35902::CPL()
{
  // Complement A Register
  //
  set_flag_n(true);
  set_flag_h(true);

  reg.a = ~reg.a;
}

void LR35902::CCF()
{
  // Complement Carry Flag

  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(!get_flag_c());
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADD_R_R()
{
  set_flag_z(reg.*R1 + reg.*R2 == 0);
  set_flag_n(false);
  set_flag_h((reg.*R1&0xf) + (reg.*R2&0xf) > 0xf);
  set_flag_c(reg.*R1 + reg.*R2 > 0xff);

  reg.*R1 += reg.*R2;
}

void LR35902::ADD_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(reg.a + hl == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (hl&0xf) > 0xf);
  set_flag_c(reg.a + hl > 0xff);

  reg.a += hl;
}

void LR35902::ADD_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);

  set_flag_z(reg.a + n == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (n&0xf) > 0xf);
  set_flag_c(reg.a + n > 0xff);

  reg.a += n;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::ADC_R_R()
{
  uint add = reg.*R2 + get_flag_c();

  set_flag_z(reg.*R1 + add == 0);
  set_flag_n(false);
  set_flag_h((reg.*R1&0xf) + (add&0xf) > 0xf);
  set_flag_c(reg.*R1 + add > 0xff);

  reg.*R1 += add;
}

void LR35902::ADC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint add = hl + get_flag_c();

  set_flag_z(reg.a + add == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (add&0xf) > 0xf);
  set_flag_c(reg.a + add > 0xff);

  reg.a += add;
}

void LR35902::ADC_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);
  uint add = n + get_flag_c();

  set_flag_z(reg.a + add == 0);
  set_flag_n(false);
  set_flag_h((reg.a&0xf) + (add&0xf) > 0xf);
  set_flag_c(reg.a + add > 0xff);

  reg.a += add;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SUB_R_R()
{
  set_flag_z(reg.*R1 - reg.*R2 == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) >= (reg.*R2&0xf));
  set_flag_c(reg.*R1 >= reg.*R2)            ;

  reg.*R1 -= reg.*R2;
}

void LR35902::SUB_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(reg.a - hl == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (hl&0xf));
  set_flag_c(reg.a >= hl)            ;

  reg.a -= hl;
}

void LR35902::SUB_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);

  set_flag_z(reg.a - n == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (n&0xf));
  set_flag_c(reg.a >= n)            ;

  reg.a -= n;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::SBC_R_R()
{
  uint sub = reg.*R2 + get_flag_c();

  set_flag_z(reg.*R1 - sub == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) >= (sub&0xf));
  set_flag_c(reg.*R1 >= sub)            ;

  reg.*R1 -= sub;
}

void LR35902::SBC_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  uint sub = hl + get_flag_c();

  set_flag_z(reg.a - sub == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (sub&0xf));
  set_flag_c(reg.a >= sub)            ;

  reg.a -= sub;
}

void LR35902::SBC_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);
  uint sub = n + get_flag_c();

  set_flag_z(reg.a - sub == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (sub&0xf));
  set_flag_c(reg.a >= sub)            ;

  reg.a -= sub;
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::AND_R_R()
{
  reg.*R1 &= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

void LR35902::AND_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a &= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

void LR35902::AND_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);
  reg.a &= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(true);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::XOR_R_R()
{
  reg.*R1 ^= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::XOR_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a ^= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::XOR_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);
  reg.a ^= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::OR_R_R()
{
  reg.*R1 |= reg.*R2;

  set_flag_z(reg.*R1 == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::OR_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);
  reg.a |= hl;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

void LR35902::OR_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);
  reg.a |= n;

  set_flag_z(reg.a == 0);
  set_flag_n(false);
  set_flag_h(false);
  set_flag_c(false);
}

template <u8 LR35902::Reg::*R1, u8 LR35902::Reg::*R2>
void LR35902::CP_R_R()
{
  set_flag_z(reg.*R1 - reg.*R2 == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.*R1&0xf) >= (reg.*R2&0xf));
  set_flag_c(reg.*R1 >= reg.*R2)            ;
}

void LR35902::CP_a_hladdr()
{
  u8 hl = memory.get8(reg.hl);

  set_flag_z(reg.a - hl == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (hl&0xf));
  set_flag_c(reg.a >= hl)            ;
}

void LR35902::CP_a_d8()
{
  u8 n = memory.get8(reg.pc + 1);

  set_flag_z(reg.a - n == 0)        ;
  set_flag_n(true);
  set_flag_h((reg.a&0xf) >= (n&0xf));
  set_flag_c(reg.a >= n)            ;
}

template <u16 LR35902::Reg::*R>
void LR35902::INC_RR()
{
  (reg.*R)++;
}

template <u16 LR35902::Reg::*R>
void LR35902::DEC_RR()
{
  (reg.*R)--;
}

template <u16 LR35902::Reg::*R1, u16 LR35902::Reg::*R2>
void LR35902::ADD_RR_RR()
{
  set_flag_n(false);
  set_flag_h((reg.*R1&0xfff) + (reg.*R2&0xfff) > 0xfff);
  set_flag_c(reg.*R1 + reg.*R2 > 0xffff);

  reg.*R1 += reg.*R2;
}

void LR35902::ADD_SP_r8()
{
  u8 n = memory.get8(reg.pc + 1);

  set_flag_z(false);
  set_flag_n(false);
  set_flag_h((reg.sp&0xfff) + (n&0xfff) > 0xfff);
  set_flag_c(reg.sp + n > 0xffff);

  reg.sp += n;
}
