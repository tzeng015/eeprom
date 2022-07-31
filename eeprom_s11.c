/******************************************/
/* This is EEPROM test smple              */
/* create 2022/07/01                      */
/* 4K  = 1 page . 64k = 16 page           */
/* gcc -o eeprom_s10 eeprom_s10.c         */
/* Input data is 0x00 , oxFF , 0x55, 0xaa */
/******************************************/
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define DATA_SIZE 4096
#define EEPROM_PAGE 16
#define EEPROM_DEVICE "/sys/bus/i2c/devices/i2c-0/0-0050/eeprom"

static void dump_data(char *data, unsigned int size) {
    int i = 0;
    for(i = 0; i < size; i++)
    {
        if(i%16 == 0)
            printf("\n");
            printf("%02x ", *data++);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int fd , ret_val = 0, i = 0, test_date;
    if((argc < 2) || ((argc > 3)))    
    {        
        printf("no argument or error argument !\n");        
        return 0;    
    }    
    if ((argc == 2))		
	{	
        printf("input test data is %s\r\n",argv[1]);
        if(strcmp(argv[1], "0x00") == 0)
        {
            test_date = 0x00;     
        }
        else if(strcmp(argv[1], "0x55") == 0)
        {
            test_date = 0x55;
        }
        else if((strcmp(argv[1], "0xaa") == 0) || (strcmp(argv[1], "0xAA") == 0))
        {
            test_date = 0xaa;
        }
        else if((strcmp(argv[1], "0xff") == 0) || (strcmp(argv[1], "0xFF") == 0))
        {
            test_date = 0xff;  
        }
        else
        {
            printf("test data error , please try 0x55 , 0xaa , 0x00 , 0xff\r\n");  
            return 0;
        }
    }
    printf("test data output is %x\n",test_date);  
    char *write_des = malloc(DATA_SIZE);
    if (write_des == NULL)
        return -1;
    char *read_data = malloc(DATA_SIZE);
    if (read_data == NULL){
        free(write_des);
        return -1;
    }
    memset(write_des, test_date, DATA_SIZE);
    fd = open(EEPROM_DEVICE, O_RDWR);
    if (fd != -1)
    {
        printf("open device OK\r\n");
        for (i = 0 ; i < EEPROM_PAGE ; i++)
        {
            //write start 0
            if (lseek(fd, DATA_SIZE*i, SEEK_SET) == (DATA_SIZE*i))
            {
                ret_val = write(fd, write_des, DATA_SIZE);
                printf("the write return val is %d and lseek right location\r\n",ret_val);
                if (ret_val == DATA_SIZE)
                {
                    printf("write data %d round Ok\r\n", i);
                    //read start 0
                    if(lseek(fd, DATA_SIZE*i, SEEK_SET) == (DATA_SIZE*i))
                    {
                        if (read(fd, read_data, DATA_SIZE) == DATA_SIZE)
                        {
                            //dump_data(read_data, DATA_SIZE);
                            printf(" read date %d round ok lseek right location\r\n", i);
                            if (memcmp(read_data, write_des, DATA_SIZE) == 0)
                            {
                                printf("data cmp ok\r\n");
                            }
                            else
                            {
                                printf("data error\r\n");
                            }
                        }
                    }
                }
            }
            memset(write_des, 0x00, DATA_SIZE);
            memset(read_data, 0x00, DATA_SIZE);
        }
        close(fd);
    }
    free(write_des);
    free(read_data);
    printf("\r\n");
}
