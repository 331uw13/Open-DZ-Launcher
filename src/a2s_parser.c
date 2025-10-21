
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>



static uint16_t read_u16(uint8_t* buf) {
    return buf[0] | (buf[1] << 8);
}

static uint32_t read_u32(uint8_t* buf) {
    return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static int count_bits(uint16_t i) {
    int count = 0;
    while(i) {
        count += i&1;
        i >>= 1;
    }
    return count;
}




void parse_a2s_rules(uint8_t* buffer, size_t size) {


    size_t offset = 0;


    uint8_t protocol_version = 0;
    uint8_t overflow_flags = 0;
    uint16_t dlc_flags = 0;
    //uint32_t dlc_hash = 0;
    uint8_t mods_count = 0;

    // ID length (byte)
    // Mod steam ID (uint32)
    // Mod name (max 256)

    uint8_t signature_count = 0;

    protocol_version = buffer[offset++];
    overflow_flags = buffer[offset++];

    dlc_flags = read_u16(buffer + offset);
    offset += sizeof(uint32_t);

    mods_count = buffer[offset++];


    printf("Protocol version: %i\n", protocol_version);
    printf("Overflow flags: %i\n", overflow_flags);
    printf("DLC flags: %i | dlc: %s\n", dlc_flags, (count_bits(dlc_flags) == 1) ? "yes" : "no");
    printf("Mods count: %i\n", mods_count);


    /*
    uint32_t mod_hash = read_u32(buffer + offset);
    offset += sizeof(uint32_t);

    uint8_t id_len = buffer[offset++];

    printf("mod hash: %i\n", mod_hash);
    printf("mod idlen: %i\n", id_len);

    uint32_t modid_u32 = read_u32(buffer + offset);
    offset += sizeof(uint32_t);

    printf("%i\n", modid_u32);
    */


    
    for(int mi = 0; mi < mods_count; mi++) {

        uint32_t mod_hash = read_u32(buffer + offset); 
        offset += sizeof(uint32_t);

        uint8_t modid_len = buffer[offset++];

        printf("\033[90mmodid_len: %i\033[0m\n", modid_len);
        // For now assume all mod id lengths are the same. 4 bytes.
        if(modid_len != 4) {
            fprintf(stderr,
                    "\033[31m\n"
                    "=================================================================\n"
                    " [ WARNING ]: Mod id length of %i bytes are not supported\n"
                    "              by the current version of this software.\n"
                    "=================================================================\n"
                    "\033[0m\n", modid_len);
            return;
        }

        // note: 10 bytes before mod name.


        // Save mod id for debugging..
        uint8_t mod_id_bytes[4] = { 0 };
        memmove(mod_id_bytes, buffer + offset, 4);
        offset += 4;

        //uint32_t mod_workshop_id = read_u32(buffer + offset);
        //offset += sizeof(uint32_t);

        uint8_t mod_name_len = buffer[offset++];
        // TODO: Check mod name length for safety. it cant be greater than 256 bytes.

        char mod_name[256] = { 0 };
        
        // Read mod name.
        for(int k = 0; k < mod_name_len; k++) {
            char ch = (char)buffer[offset];
            if(ch >= 0x20 && ch <= 0x7E) {
                mod_name[k] = ch;
            }
            else {
                k--;
            }
            offset++;
        }

        printf("(%i)'%s' \033[32m", mod_name_len, mod_name);

        
        for(int i = 0; i < 4; i++) {
            printf("%02X ", mod_id_bytes[i]);
        }

        uint32_t mod_workshop_id = read_u32(mod_id_bytes);
        

        //memmove(&mod_workshop_id, mod_id_bytes, 4);

        printf("\033[0m | %u\n", mod_workshop_id);

    
    }
    





}



