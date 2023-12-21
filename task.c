#include <stdint.h>

struct {
	uint32_t tss, *esp0;
	uint16_t ss0, _[46], io;
} tss = {.ss0 = 16,.io = 103 };

struct {
	uint32_t stack[100], esp;
} tasks[5], *task = tasks;

void clock(void)
{
	asm("pusha\npush %ds\npush %es");
 asm("mov %%esp, %0\nout %1, $0x20": "=m"(task->esp):"a"((uint8_t)
	    0x20));
	if (++task == tasks + sizeof(tasks) / sizeof(tasks[0]))
		task = tasks;

	tss.esp0 = &task->esp;
	asm("mov %0, %%esp\npop %%es\npop %%ds\npopa\npop %%ebp\niret"::"m"
	    (task->esp));
}

void empty(void)
{
	asm("pop %ebp\niret");
}

void setup_idt(void)
{
	static uint64_t idt[256];
	for (int i = 0; i < 256; ++i)
		idt[i] = 0x8e0000080000 + (uint32_t) empty;

	idt[8] = 0x8e0000080000 + (uint32_t) clock;
	uint16_t m[3] = { sizeof(idt) - 1 };
	*(uint64_t **) (m + 1) = idt;
	asm("ltr %0\n" "lidt %1\n" "sti"::"r"((uint16_t) 40), "m"(m));
}

void setup_gdt(void)
{
	static uint64_t gdt[6] = {
		0,
		0xcf98000000ffff,
		0xcf92000000ffff,
		0xcff8000000ffff,
		0xcff2000000ffff,
	};

	gdt[5] = 0x890000000067 + ((uint32_t) & tss << 16);
	uint16_t m[3] = { sizeof(gdt) - 1 };
	*(uint64_t **) (m + 1) = gdt;
	asm("lgdt %0"::"m"(m));
}

void start(int i)
{
	i *= 160;
	i += 0xb8000;
	while (1)
		asm("incb (%0)"::"r"(i));
}

void init_task(int i)
{
	uint32_t esp = 0x100000 + 0x1000 * i - 4;
	*(int *)esp = i;
	tasks[i].stack[0] = 35;
	tasks[i].stack[1] = 35;
	tasks[i].stack[11] = (uint32_t) start;
	tasks[i].stack[12] = 27;
	tasks[i].stack[13] = 0x202;
	tasks[i].stack[14] = esp - 4;
	tasks[i].stack[15] = 35;
	tasks[i].esp = (uint32_t) tasks[i].stack;
}

void _start(void)
{
	for (int i = 1; i < sizeof(tasks) / sizeof(tasks[0]); ++i)
		init_task(i);

	setup_gdt();
	setup_idt();
	start(0);
}
