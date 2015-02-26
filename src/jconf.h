//
//  jconf.h
//  jedisocks
//
//  Created by jedihy on 15-2-25.
//  Copyright (c) 2015年 jedihy. All rights reserved.
//

#ifndef jedisocks_jconf_h
#define jedisocks_jconf_h
typedef struct {
    uint16_t localport;
    uint16_t serverport;
    char* server_address;
    char* local_address;
} conf_t;

extern void read_conf(char* configfile, conf_t* conf);
extern int server_validate_conf(conf_t* conf);
extern int local_validate_conf(conf_t* conf);
#endif
