/*!
 *\brief commands act similarly to commands from the assembly language
 * 
 * command PRINT_PHRASE was created specifically for the output of the Square_equation program text
 * command draw currently under development
 */
DEF_CMD (HLT, 0, 0,
        {
        #ifdef DEBUG_CPU
            fclose (CPU_log);
        #endif
    
            stack_dtor (&stk) ASSERTED;
            stack_dtor (&call_stk) ASSERTED;
            log_file_Dtor ();
    
            return CPU_Errors_NICE;
        }
)

DEF_CMD (PUSH, 1, 1,
        {
            num_type arg = 0;
            if (cur_cmd & 0x20) arg += cmd_arr[++ip];
            if (cur_cmd & 0x40) arg += regs[(int) cmd_arr[++ip]];
            if (cur_cmd & 0x80) arg = RAM[(int) arg];
            
            stack_push (&stk, arg) ASSERTED;
        }
)

DEF_CMD (POP, 2, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
                
            num_type index = 0;
            if (cur_cmd & 0x80)
            {
                if (cur_cmd & 0x20) index += cmd_arr[++ip];
                if (cur_cmd & 0x40) index += regs[(int) cmd_arr[++ip]];
                //printf ("index = %d value = %d\n", index, value1);
                RAM[(int) index] = value1;
            }
            else if (cur_cmd & 0x40)
            {
                index += cmd_arr[++ip];
                regs[(int) index] = value1;
            }
            else
            {
                printf ("Pop value error\n");
                return CPU_Errors_POP_VALUE_ERR;
            }
            
            value1 = 0;
        }
)

DEF_CMD (ADD, 3, 0,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            value1 += value2;
            stack_push (&stk, value1) ASSERTED;
            value1 = value2 = 0;
        }
)

DEF_CMD (SUB, 4, 0,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            value2 -= value1;
            stack_push (&stk, value2) ASSERTED;
            value1 = value2 = 0;
        }
)

DEF_CMD (MUL, 5, 0,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            value1 *= value2;
            stack_push (&stk, value1) ASSERTED;
            value1 = value2 = 0;
        }
)

DEF_CMD (DIV, 6, 0,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            value1 = value2 / value1;
            stack_push (&stk, value1) ASSERTED;
            value1 = value2 = 0;
        }
)

DEF_CMD (OUT, 7, 0,
        {
            stack_pop (&stk, &value1) ASSERTED;
            printf ("Out value: " NUM_FMT "\n", value1);
            value1 = value2 = 0;
        }
)

DEF_CMD (DUMP, 8, 0,
        {
            dump_func (cmd_arr, (size_t) cmd_amount, (size_t) ip, &stk);
        }
) 

DEF_CMD (IN, 9, 0,
        {
             if (scanf (NUM_FMT, &value1) != 1)
             {
                 printf ("Incorrect input in the IN command\n");
                 
                #ifdef DEBUG_CPU
                fclose (CPU_log);
                #endif
    
                stack_dtor (&stk) ASSERTED;
                stack_dtor (&call_stk) ASSERTED;
                log_file_Dtor ();
    
                return CPU_Errors_NICE;
             }
             
             stack_push (&stk, value1) ASSERTED;
             value1 = 0;
        }
)

DEF_CMD (DUP, 10, 0, //duplicates the last value in the stack
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_push (&stk, value1) ASSERTED;
            stack_push (&stk, value1) ASSERTED;
            value1 = 0;
        }
)

DEF_CMD (JMP, 11, 1,
        {
            ip = (ssize_t) cmd_arr[ip + 1] - 1; // correction for the for counter
        }
)

DEF_CMD (JB, 12, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (double_more(value1, value2))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (JBE, 13, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (double_more(value1, value2) || double_equal (value1, value2))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (JA, 14, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (double_more(value2, value1))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (JAE, 15, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (double_more(value2, value1) || double_equal (value1, value2))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (JE, 16, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (double_equal (value2, value1))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (JNE, 17, 1,
        {
            stack_pop (&stk, &value1) ASSERTED;
            stack_pop (&stk, &value2) ASSERTED;
            if (!double_equal (value1, value2))
                ip = (ssize_t) cmd_arr[ip + 1] - 1;
            else
                ip++;
            value1 = value2 = 0;
        }
)

DEF_CMD (CALL, 18, 1,
        {
            stack_push (&call_stk, (double) ip + 1) ASSERTED; //+1, not +2 because of for
            ip = (ssize_t) cmd_arr[ip + 1] - 1;
        }
)

DEF_CMD (RET, 19, 0,
        {
            num_type ip_ret = 0;
            stack_pop (&call_stk, &ip_ret) ASSERTED;
            
            ip = (ssize_t) ip_ret;
        }
)

DEF_CMD (SQRT, 20, 0, 
        {
             stack_pop (&stk, &value1) ASSERTED;
             value1 = sqrt (value1);
             stack_push (&stk, value1) ASSERTED;
             value1 = 0;
        }
)

DEF_CMD (PRINT_PHRASE, 21, 1,
        {
             ip++;
             switch ((int) cmd_arr[ip])
             {
                case 0:
                    printf ("Hello! It's a programm to solve square equations. Please, enter "
                            "coefficients a, b, c of ax^2 + bx + c = 0 equation form.\n");
                    break;
                case 1:
                    printf ("One root: \n");
                    break;
                case 2:
                    printf ("Two roots: \n");
                    break;
                case 3:
                    printf ("Infinite roots\n");
                    break;
                case 4:
                    printf ("Discriminant < 0. No roots\n");
                    break;
                case 5:
                    printf ("No roots\n");
                    break;
                case 6:
                    printf ("Enter the number whose factorial you want to calculate\n");
                    break;
                case 7:
                    printf ("Enter the radius of the circle\n");
                    break;
                case 8:
                    printf ("Negative number. Can't calculate this\n");
                    break;
                default:
                    printf ("meow\n");
                    break;
             }
        }
)

DEF_CMD (draw, 22, 0,
        {
             stack_pop (&stk, &value1);
             int VIDEO_MEM_END = (int) (value1 * value1 + VIDEO_MEM_START);
             if (VIDEO_MEM_END > RAM_SIZE)
             {
                 VIDEO_MEM_END = RAM_SIZE;
                 value1 = 19;
             }
             for (int i = 0; i < value1; i++)
             {
                for (int j = 0; j < (int) value1 * 2 + 1; j++)
                    putchar ((int) RAM[VIDEO_MEM_START + (int)(i*value1 * 2 + 1) + j] + '*');
                putchar('\n');
             }
             value1 = 0;
        }
)

DEF_CMD (floor, 23, 0,
        {
            stack_pop (&stk, &value1);
            value1 = floor (value1);
            stack_push (&stk, value1);
            value1 = 0;
        }
)

DEF_CMD (sin, 24, 0, 
        {
             stack_pop (&stk, &value1);
             value1 = sin (value1);
             stack_push (&stk, value1);
             value1 = 0;
        }
)

DEF_CMD (cos, 25, 0,
        {
             stack_pop (&stk, &value1);
             value1 = cos (value1);
             stack_push (&stk, value1);
             value1 = 0;
        }
)
