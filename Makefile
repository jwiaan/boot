disk: boot task
	cp boot disk
	dd if=task of=$@ oflag=append conv=notrunc,sync

boot: boot.s
	nasm $<

task: task.o
	ld -melf_i386 -Ttext 0xa000 -o$@ $^

task.o: task.c
	cc -c -m32 -nostdlib -fno-pie -Wall $<

.PHONY: clean
clean:
	rm -f *.o boot task disk
