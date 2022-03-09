#ifndef __UTILS_H__
#define __UTILS_H__

//extern char* itoa(int value, char* buffer, int base);

//float bytes_to_float(__uint8_t hsb, __uint8_t hmsb, __uint8_t lmsb, __uint8_t lsb);

extern char *return_default_labe(int mp, char * dir, int cp);

extern uint32_t get_time_millis(void);

extern unsigned long current_timestamp();

extern int get_cpu_load();

#endif /*__UTILS_H__*/