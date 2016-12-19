#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "logger.h"
#include "hash.h"
#include "shmcache.h"

int main(int argc, char *argv[])
{
	int result;
    struct shmcache_config config;
    struct shmcache_context context;
    struct shmcache_buffer key;
    struct shmcache_buffer value;
    const int ttl = 600;
	
	log_init();
	g_log_context.log_level = LOG_DEBUG;
   
    printf("sizeof(struct shm_hash_entry): %d\n",
            (int)sizeof(struct shm_hash_entry));

    memset(&config, 0, sizeof(config));

    strcpy(config.filename, "/tmp/shmcache.dat");
    config.max_memory = 4 * 1024 * 1024;
    config.segment_size = 1 * 1024 * 1024;
    config.max_key_count = 10000;
    config.max_value_size = 64 * 1024;
    config.type = SHMCACHE_TYPE_SHM;  //shm or mmap

    config.va_policy.avg_key_ttl = 600;
    config.va_policy.discard_memory_size = 128;
    config.va_policy.max_fail_times = 5;

    config.lock_policy.trylock_interval_us = 1000;
    config.lock_policy.detect_deadlock_interval_ms = 1000;
    config.hash_func = simple_hash;

    if ((result=shmcache_init(&context, &config)) != 0) {
        return result;
    }

    key.data = "key";
    key.length = strlen(key.data);
    value.data = "this is a test";
    value.length = strlen(value.data);
    result = shmcache_set(&context, &key, &value, ttl);
    if (result == 0) {
        printf("set OK\n");
    } else {
        printf("set fail, errno: %d\n", result);
    }

	return 0;
}

