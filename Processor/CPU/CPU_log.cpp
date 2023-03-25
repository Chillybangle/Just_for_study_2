#include <stdio.h>

void binary_out (int cmd, FILE* CPU_log);
void cmd_dump (int cmd, FILE* CPU_log);

void cmd_dump (int cmd, FILE* CPU_log)
{
    if (cmd < 0)
        fprintf (CPU_log, "cmd < 0. Is it okay?\n");
    fprintf (CPU_log, "%d(%x) ", cmd, (unsigned int) cmd);
    binary_out (cmd, CPU_log);
    fprintf (CPU_log, " (%s %s %s, #%d)\n\n", (cmd & 0x80)? "M" : "m", (cmd & 0x40)? "R" : "r", (cmd & 0x20)? "I" : "i", cmd & 0x1F);
}

void binary_out (int cmd, FILE* CPU_log)
{
    for (int i = 0; i < 8; i++) {
        fprintf(CPU_log, "%c", (cmd & 0x80) ? '1' : '0');
        cmd <<= 1;
        if (i == 3)
            fputc (' ', CPU_log);
    }
}
