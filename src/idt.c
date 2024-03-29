#include "idt.h"

int is_extended = 0;
idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

void init_idt()
{
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = (uint64_t)idt_entries;

  for (uint64_t i = 0; i < sizeof(idt_entry_t) * 256; ++i) {
    ((uint8_t*)idt_entries)[i] = 0;
  }

  #define PIC1 0x20
  #define PIC2 0xA0
  #define ICW1 0x11
  #define ICW4 0x01

  // Begin initialization
  outb(PIC1, 0x11);
  outb(PIC2, 0x11);

  // Remap irqs to master -> 0x20-0x27
  //  and slave -> 0x28-0x2F
  outb(PIC1 + 1, 0x20);
  outb(PIC2 + 1, 0x28);

  outb(PIC1 + 1, 4);
  outb(PIC2 + 1, 2);

  outb(PIC1 + 1, 0x01);
  outb(PIC2 + 1, 0x01);

  outb(PIC1 + 1, 0);
  outb(PIC2 + 1, 0);

  idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
  idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
  idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
  idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
  idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
  idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
  idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
  idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
  idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
  idt_set_gate(9, (uint64_t)isr9, 0x08, 0x8E);
  idt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
  idt_set_gate(11, (uint64_t)isr11, 0x08, 0x8E);
  idt_set_gate(12, (uint64_t)isr12, 0x08, 0x8E);
  idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);
  idt_set_gate(15, (uint64_t)isr15, 0x08, 0x8E);
  idt_set_gate(16, (uint64_t)isr16, 0x08, 0x8E);
  idt_set_gate(17, (uint64_t)isr17, 0x08, 0x8E);
  idt_set_gate(18, (uint64_t)isr18, 0x08, 0x8E);
  idt_set_gate(19, (uint64_t)isr19, 0x08, 0x8E);
  idt_set_gate(20, (uint64_t)isr20, 0x08, 0x8E);
  idt_set_gate(21, (uint64_t)isr21, 0x08, 0x8E);
  idt_set_gate(22, (uint64_t)isr22, 0x08, 0x8E);
  idt_set_gate(23, (uint64_t)isr23, 0x08, 0x8E);
  idt_set_gate(24, (uint64_t)isr24, 0x08, 0x8E);
  idt_set_gate(25, (uint64_t)isr25, 0x08, 0x8E);
  idt_set_gate(26, (uint64_t)isr26, 0x08, 0x8E);
  idt_set_gate(27, (uint64_t)isr27, 0x08, 0x8E);
  idt_set_gate(28, (uint64_t)isr28, 0x08, 0x8E);
  idt_set_gate(29, (uint64_t)isr29, 0x08, 0x8E);
  idt_set_gate(30, (uint64_t)isr30, 0x08, 0x8E);
  idt_set_gate(31, (uint64_t)isr31, 0x08, 0x8E);
  idt_set_gate(32, (uint64_t)isr32, 0x08, 0x8E);
  idt_set_gate(33, (uint64_t)isr33, 0x08, 0x8E);
  idt_set_gate(34, (uint64_t)isr34, 0x08, 0x8E);
  idt_set_gate(35, (uint64_t)isr35, 0x08, 0x8E);
  idt_set_gate(36, (uint64_t)isr36, 0x08, 0x8E);
  idt_set_gate(37, (uint64_t)isr37, 0x08, 0x8E);
  idt_set_gate(38, (uint64_t)isr38, 0x08, 0x8E);
  idt_set_gate(39, (uint64_t)isr39, 0x08, 0x8E);
  idt_set_gate(40, (uint64_t)isr40, 0x08, 0x8E);

  idt_flush((uint64_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
  idt_entries[num].offset_1 = base & 0xFFFF;
  idt_entries[num].offset_2 = (base >> 16) & 0xFFFF;
  idt_entries[num].offset_3 = (base >> 32) & 0xFFFFFFFF;

  idt_entries[num].selector = sel;
  idt_entries[num].zero = 0;
  idt_entries[num].ist = 0;
  idt_entries[num].type_attr = flags;
}

void isr_handler(uint64_t n, uint64_t error)
{
  int code = 0;
  switch (n)
  {
    case 0:
      panic("Divided by zero!\n");
      break;
    // System timer
    case 0x20:
      break;
    // Keyboard
    case 0x21:
      code = inb(0x60);
      if (code == 0xE0)
      {
        is_extended = 1;
        return;
      }
      if (is_extended)
      {
        code = code << 8;
      }
      is_extended = 0;
      printf("Got code %x\n", code);
      break;
    default:
      printf("Interrupt %x called.\n", n);
      break;
  }
}
