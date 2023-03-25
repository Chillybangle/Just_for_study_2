#ifndef CPU_H
#define CPU_H

#define DEBUG_CPU

enum CPU_Errors preprocessing_check (FILE* program_file, ssize_t* cmd_amount);
num_type* read_file (FILE* program_file, ssize_t cmd_amount);
enum CPU_Errors cmd_processing (num_type* cmd_arr, ssize_t cmd_amount);
void error_out (enum CPU_Errors result);
void dump_func (num_type* arr, size_t cmd_amount, size_t cur_ip, const Stack* stk);
void cmd_dump (int cmd, FILE* CPU_log);

#endif
