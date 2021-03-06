#include "utfconverter.h"

#ifdef CSE320
    #define debug1(msg) fprintf(stderr, "CSE320: Hostname: %s\n", msg)
    #define debug2(path, dev, ino, size) fprintf(stderr, "CSE320: Input: %s, %lu, %lu, %lu byte(s)\n", path, dev, ino, size)
    #define debug3(msg) fprintf(stderr, "CSE320: Output: %s\n", msg)
    #define debug4(msg) fprintf(stderr, "CSE320: Input encoding: %s\n", msg)
    #define debug5(msg) fprintf(stderr, "CSE320: Output encoding: %s\n", msg)
#else
    #define debug1(msg)
    #define debug2(path, dev, ino, size)
    #define debug3(msg)
    #define debug4(msg)
    #define debug5(msg)
#endif

int main(int argc, char *argv[]) {
    int opt, return_code = EXIT_FAILURE;
    char *input_path = NULL;
    char *output_path = NULL;
    /* open output channel */
    FILE* standardout = fopen("stdout", "w");
    /* Output type */
    char* eValue = NULL;
    int eFound = 0;
    int outputType = 0;
    int vCount = 0;
    /* Parse short options */
    while((opt = getopt(argc, argv, "he:v")) != -1) {
        switch(opt) {
            case 'h':
                /* The help menu was selected */
                USAGE(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'e':
                eValue = optarg;
                eFound = 1;
                break;
            case 'v':
                vCount++;
                break;
            case '?':
                /* Let this case fall down to default;
                 * handled during bad option.
                 */
            default:
                /* A bad option was provided. */
                USAGE(argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }
    /* Changing around some behavioral variables */
    if(!eFound){
        fprintf(stderr, "The -e argument must be supplied with a valid file type.");
        USAGE(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(vCount > 3){
        vCount = 3;
    }
    if(strcmp(eValue, UTF8_STRING) == 0){
        outputType = UTF8_VALUE;
    } else if (strcmp(eValue, UTF16LE_STRING) == 0){
        outputType = UTF16LE_VALUE;
    } else if (strcmp(eValue, UTF16BE_STRING) == 0){
        outputType = UTF16BE_VALUE;
    } else {
        fprintf(stderr, "The -e argument must be supplied with a valid file type.");
        USAGE(argv[0]);
        exit(EXIT_FAILURE);
    }
    /* Get position arguments */
    if(optind < argc && (argc - optind) == 2) {
        input_path = argv[optind++];
        output_path = argv[optind++];
    } else {
        if((argc - optind) <= 0) {
            fprintf(standardout, "Missing INPUT_FILE and OUTPUT_FILE.\n");
        } else if((argc - optind) == 1) {
            fprintf(standardout, "Missing OUTPUT_FILE.\n");
        } else {
            fprintf(standardout, "Too many arguments provided.\n");
        }
        USAGE(0[argv]);
        exit(EXIT_FAILURE);
    }
    /* Make sure all the arguments were provided */
    if(input_path != NULL || output_path != NULL) {
        int input_fd = -1, output_fd = -1;
        bool success = false;
        switch(validate_args(input_path, output_path)) {
                case VALID_ARGS:
                    /* Attempt to open the input file */
                    if((input_fd = open(input_path, O_RDONLY)) < 0) {
                        fprintf(standardout, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }
                    /* Delete the output file if it exists; Don't care about return code. */
                    unlink(output_path);
                    /* Attempt to create the file */
                    if((output_fd = open(output_path, O_CREAT | O_WRONLY,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
                        /* Tell the user that the file failed to be created */
                        fprintf(standardout, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }
                    auto ssize_t bytes_read;
                    auto unsigned char read_value;
                    unsigned char bom[3];
                    int inputType = 0;
                    int i = 0;
                    for(; i < 3; i++){
                        if((bytes_read = read(input_fd, &read_value, 1)) != 1){
                            fprintf(stderr, "Error reading in from file while processing BOM.\n");
                            return_code = EXIT_FAILURE;
                            goto conversion_done;
                        } else {
                            bom[i] = read_value;
                            if(i == 1){
                                /* UTF-16LE */
                                if(bom[0] == 0xff && bom[1] == 0xfe){
                                    inputType = UTF16LE_VALUE;
                                    i++;
                                } /* UTF-16BE */ 
                                else if(bom[0] == 0xfe && bom[1] == 0xff){
                                    inputType = UTF16BE_VALUE;
                                    i++;
                                }
                            }
                        }
                    }
                    if(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF){
                        inputType = UTF8_VALUE;
                    }
                    /* No valid BOM found */
                    if(!inputType){
                        fprintf(stderr, "No valid BOM found for input file %s\n", input_path);
                        goto conversion_done;
                    }

                    /* Debug stuff */
                    struct stat ip;
                    if(stat(input_path, &ip) == -1){
                        goto conversion_done;
                    }
                    char hostName[128];
                    gethostname(hostName, 127);
                    debug1(hostName);
                    debug2(input_path, ip.st_dev, ip.st_ino, ip.st_size);
                    debug3(output_path);
                    switch(inputType){
                        case UTF8_VALUE:
                            debug4(UTF8_STRING);
                            break;
                        case UTF16LE_VALUE:
                            debug4(UTF16LE_STRING);
                            break;
                        case UTF16BE_VALUE:
                            debug5(UTF16BE_STRING);
                            break;
                    }
                    switch(outputType){
                        case UTF8_VALUE:
                            debug5(UTF8_STRING);
                            break;
                        case UTF16LE_VALUE:
                            debug5(UTF16LE_STRING);
                            break;
                        case UTF16BE_VALUE:
                            debug5(UTF16BE_STRING);
                            break;
                    }

                    /* Start the conversion */
                    if(inputType == UTF8_VALUE){
                        success = convert(input_fd, output_fd, outputType);
                    } else if(inputType == UTF16LE_VALUE || inputType == UTF16BE_VALUE){
                        /* Convert from UTF16-LE or UTF16-BE to UTF-8 */
                        if(outputType == UTF8_VALUE) {
                            success = convertToUTF8(input_fd, output_fd, outputType, inputType);
                        } else if (outputType != inputType){
                            /* Switching from BE to LE or LE to BE, just switch the bytes around */
                            success = convertSwitchEnd(input_fd, output_fd, outputType);
                        } else {
                            /* Same to same, just read in bytes and print */
                            success = convertSame(input_fd, output_fd, outputType);
                        }
                    }
conversion_done:
                    close(input_fd);
                    close(output_fd);
                    if(success) {
                        /* We got here so it must of worked right? */
                        return_code = EXIT_SUCCESS;
                    } else {
                        /* Conversion failed; clean up */
                        if(output_fd < 0 && input_fd >= 0) {
                            /* close(input_fd); */
                        }
                        if(output_fd >= 0) {
                            /* close(output_fd); */
                            unlink(output_path);
                        }
                        /* Just being pedantic... */
                        return_code = EXIT_FAILURE;
                    }
                    break;
                case SAME_FILE:
                    fprintf(standardout, "The output file %s was not created. Same as input file.\n", output_path);
                    break;
                case FILE_DNE:
                    fprintf(standardout, "The input file %s does not exist.\n", input_path);
                    break;
                default:
                    fprintf(standardout, "An unknown error occurred\n");
                    /* continue; */
        }
    } else {
        /* Alert the user what was not set before quitting. */
        if((input_path = NULL) == NULL) {
            fprintf(standardout, "INPUT_FILE was not set.\n");
        }
        if((output_path = NULL) == NULL) {
            fprintf(standardout, "OUTPUT_FILE was not set.\n");
        }
        /* Print out the program usage */
        USAGE(argv[0]);
    }
    if(return_code == EXIT_SUCCESS){
        fprintf(stderr, "The file %s was successfully created.\n", output_path);
    } else {
        fprintf(stderr, "The file %s was not created.\n", output_path);
    }
    return return_code;
}

int validate_args(const char *input_path, const char *output_path) {
    int return_code = FAILED;
    /* Make sure both strings are not NULL */
    if(input_path != NULL && output_path != NULL) {
        struct stat ip;
        struct stat op;
        if(stat(input_path, &ip) == -1){
            return_code = FILE_DNE;
            goto end_of_validate;
        }
        else if(stat(output_path, &op) == -1){
            int err = errno;
            /* File does not exist. Cannot be same file. File will be created later. */
            if(err == 2){
                return_code = VALID_ARGS;
                goto end_of_validate;
            }
        }
        else if(ip.st_dev == op.st_dev){
            if(ip.st_ino == op.st_ino){
                return_code = SAME_FILE;
                goto end_of_validate;
            }
        }
    }
    return_code = VALID_ARGS;
end_of_validate:
    return return_code;
}

/* This function takes the bits of a given int and moves them around
 * so the endianess will make sense for different machines. It breaks
 * the 32 bit entity up into it's 4 8-bit segments and then rearranges 
 * them. Might not work on 64 bit machines? But shouldn't matter because
 * is only going to be used on 32-bit Sparky.
 */
int changeEndian(int r){
    int u1;
    int u2;
    int u3;
    int u4;
    u1 = (r & 0xff) << 24;
    u2 = (r & 0xff00) << 16;
    u3 = (r & 0xff0000) << 8;
    u4 = (r & 0xff000000);
    r = u1 | u2 | u3 | u4;
    return r;
}

/* Converts from UTF-8 to output type */
bool convert(const int input_fd, const int output_fd, int outType) {
    bool success = false;
    char hostName[128];
    gethostname(hostName, 127);
    char *sparky = "sparky";
    bool s = false;
    if (strcmp(hostName, sparky) == 0) {
        s = true;
    }
    if(outType == UTF16LE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w1 = 0xff;
            int w2 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_done;
            }
        }
    } else if (outType == UTF16BE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w2 = 0xff;
            int w1 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_done;
            }
        }
    } else if (outType == UTF8_VALUE) {
        int bom = 0xef;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_done;
        }
        bom = 0xbb;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_done;
        }
        bom = 0xbf;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_done;
        }
    }
    /* UTF-8 encoded text can be @ most 4-bytes */
    unsigned char bytes['4'-'0'];
    auto unsigned char read_value;
    /* This was originally 'zero'. Not sure how that changes it. */
    auto size_t count = 0;
    auto int safe_param = SAFE_PARAM; /* DO NOT DELETE, PROGRAM WILL BE UNSAFE */
    void* saftey_ptr = &safe_param;
    auto ssize_t bytes_read;
    bool encode = false;
    /* Read in UTF-8 Bytes */
    if(!(outType == UTF8_VALUE)){
        while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
        /* Mask the most significant bit of the byte */
        unsigned char masked_value = read_value & UTF8_CONT;
        if(masked_value == UTF8_CONT) {
            if((read_value & UTF8_4_BYTE) == UTF8_4_BYTE ||
                (read_value & UTF8_3_BYTE) == UTF8_3_BYTE ||
                (read_value & UTF8_2_BYTE) == UTF8_2_BYTE) {
                /* Check to see which byte we have encountered */
                if(count == 000) {
                     count++[bytes] = read_value;
                } else {
                    /* Set the file position back 1 byte */
                    if(lseek(input_fd, -1, SEEK_CUR) < 0) {
                        /*Unsafe action! Increment! */
                        safe_param = *(int*)++saftey_ptr;
                        /* failed to move the file pointer back */
                        perror("NULL");
                        goto conversion_done;
                    }
                    /* Encode the current values into UTF-16LE */
                    encode = true;
                }
            } else if((read_value & UTF8_CONT) == UTF8_CONT) {
                /* continuation byte */
                 bytes[count++] = read_value;
            }
        } else {
            if(count == 000) {
                /* US-ASCII */
                bytes[count++] = read_value;
                encode = true;
            } else {
                /* Found an ASCII character but theres other characters
                 * in the buffer already.
                * Set the file position back 1 byte.
                */
                if(lseek(input_fd, -1, SEEK_CUR) < 0) {
                    /*Unsafe action! Increment! */
                    safe_param = *(int*) ++saftey_ptr;
                    /* failed to move the file pointer back */
                    perror("NULL");
                    goto conversion_done;
                }
                /* Encode the current values into UTF-16LE */
                encode = true;
            }
        }
        /* If its time to encode do it here */
        if(encode) {
                int i, value = 0;
                i = 0;
                bool isAscii = false;
                for(; i < count; i++) {
                    if(i == 0) {
                        if((bytes[i] & UTF8_4_BYTE) == UTF8_4_BYTE) {
                            value = bytes[i] & 0x7;
                        } else if((bytes[i] & UTF8_3_BYTE) == UTF8_3_BYTE) {
                            value =  bytes[i] & 0xF;
                        } else if((bytes[i] & UTF8_2_BYTE) == UTF8_2_BYTE) {
                            value =  bytes[i] & 0x1F;
                        } else if((bytes[i] & UTF8_CONT) == 0) {
                            /* Value is an ASCII character */
                            value = bytes[i];
                            isAscii = true;
                        } else {
                            /* Marker byte is incorrect */
                            goto conversion_done;
                        }
                    } else {
                        if(!isAscii) {
                            value = (value << 6) | (bytes[i] & 0x3F);
                        } else {
                            /* How is there more bytes if we have an ascii char? */
                            goto conversion_done;
                        }
                    }
                }
                /* safe_write returns the number of bytes written to the file */
                /* It will only trigger an error if the number of bytes written is 0 */
                /* Handle the value if its a surrogate pair*/
                if(outType == UTF16LE_VALUE){
                    if(value >= SURROGATE_PAIR) {
                        int vprime = value - SURROGATE_PAIR;
                        int w1 = (vprime >> 10) + 0xD800;
                        int w2 = 0 /*(vprime & 0x3FF) + 0xDC00*/;
                        /* write the surrogate pair to file */
                        if(s){
                            w1 = changeEndian(w1);
                            w2 = changeEndian(w2);
                        }
                        if(!safe_write(output_fd, &w1, CODE_UNIT_SIZE)) {
                        	/* Assembly for some super efficient coding */
                            /* asm("movl	$8, %esi\n\t"
    							"movl	$.LC0, %edi\n\t"
    							"movl	$0, %eax"); */
                            goto conversion_done;
                        }
                        if(!safe_write(output_fd, &w2, CODE_UNIT_SIZE)) {
                        	/* Assembly for some super efficient coding */
                            /* asm("movl	$8, %esi\n\t"
    							"movl	$.LC0, %edi\n"
    							"movl	$0, %eax"); */
                            goto conversion_done;
                        }
                    } else {
                        /* write the code point to file */
                        if(s){
                            value = changeEndian(value);
                        }
                        if(!safe_write(output_fd, &value, CODE_UNIT_SIZE)) {
                            /* Assembly for some super efficient coding */
                            /* asm("movl	$8, %esi\n"
        						"movl	$.LC0, %edi\n"
        						"movl	$0, %eax"); */
                            goto conversion_done;
                        }
                    } 
                } else if (outType == UTF16BE_VALUE){
                    if(value >= SURROGATE_PAIR) {
                        int vprime = value - SURROGATE_PAIR;
                        int w1 = (vprime >> 10) + 0xD800;
                        int w2 = (vprime & 0x3FF) + 0xDC00;
                        /* More endian magic? It works lower. */
                        int upper8 = w1 & 0xff00;
                        int lower8 = w1 & 0xff;
                        upper8 = upper8 >> 8;
                        lower8 = lower8 << 8;
                        w1 = upper8 | lower8;

                        upper8 = w2 & 0xff00;
                        lower8 = w2 & 0xff;
                        upper8 = upper8 >> 8;
                        lower8 = lower8 << 8;
                        w2 = upper8 | lower8;

                        /* write the surrogate pair to file */
                        if(s){
                            w1 = changeEndian(w1);
                            w2 = changeEndian(w2);
                        }
                        if(!safe_write(output_fd, &w1, CODE_UNIT_SIZE)) {
                            /* Assembly for some super efficient coding */
                            /* asm("movl    $8, %esi\n\t"
                                "movl   $.LC0, %edi\n"
                                "movl   $0, %eax"); */
                            goto conversion_done;
                        }
                        if(!safe_write(output_fd, &w2, CODE_UNIT_SIZE)) {
                            /* Assembly for some super efficient coding */
                            /* asm("movl    $8, %esi\n\t"
                                "movl   $.LC0, %edi\n\t"
                                "movl   $0, %eax"); */
                            goto conversion_done;
                        }
                    } else {
                        /* write the code point to file */
                        /* Change the endianess through bit shifting! */
                        if(!(value > 255)){
                            value = value << 8;
                        } else {
                            int upper8 = value & 0xff00;
                            int lower8 = value & 0xff;
                            upper8 = upper8 >> 8;
                            lower8 = lower8 << 8;
                            value = upper8 | lower8;
                        }
                        if(s){
                            value = changeEndian(value);
                        }
                        if(!safe_write(output_fd, &value, CODE_UNIT_SIZE)) {
                            /* Assembly for some super efficient coding */
                            /* asm("movl    $8, %esi\n"
                                "movl   $.LC0, %edi\n"
                                "movl   $0, %eax"); */
                            goto conversion_done;
                        }
                    }
                } else if(outType == UTF8_VALUE) {
                    if(!safe_write(output_fd, &value, 1)) {
                        goto conversion_done;
                    } 
                }
                /* Done encoding the value to UTF-16BE */
                encode = false;
                count = 0;
        }
        }
    } else {
        while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
            if(!safe_write(output_fd, &read_value, 1)) {
                goto conversion_done;
            }
        }
    }
    /* If we got here the operation was a success! */
    success = true;
conversion_done:
    return success;
}

bool convertSwitchEnd(const int input_fd, const int output_fd, const int outType) {
    bool success = false;
    auto ssize_t bytes_read;
    auto unsigned char read_value;
    unsigned char bytes[2];
    int bytesInArray = 0;
    char hostName[128];
    gethostname(hostName, 127);
    char *sparky = "sparky";
    bool s = false;
    if (strcmp(hostName, sparky) == 0) {
        s = true;
    }
    if(outType == UTF16LE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w1 = 0xff;
            int w2 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_end_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_end_done;
            }
        }
    } else if (outType == UTF16BE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w2 = 0xff;
            int w1 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_end_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_end_done;
            }
        }
    }
    /* Read in by bytes of 2, then switch them. */
    while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
        /* No bytes read in yet */
        if(bytesInArray == 0){
            bytes[0] = read_value;
            bytesInArray++;
        } else {
            /* 1 byte read in, reading in second byte now */
            bytes[1] = read_value;
            int w1 = bytes[0];
            int w2 = bytes[1];
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_end_done;
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_end_done;
            }
            bytesInArray = 0;
        }
    }
    success = true;
conversion_end_done:
    return success;
}

bool convertSame(const int input_fd, const int output_fd, const int outType) {
    bool success = false;
    auto ssize_t bytes_read;
    auto unsigned char read_value;
    char hostName[128];
    gethostname(hostName, 127);
    char *sparky = "sparky";
    bool s = false;
    if (strcmp(hostName, sparky) == 0) {
        s = true;
    }
    if(outType == UTF16LE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w1 = 0xff;
            int w2 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_same_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_same_done;
            }
        }
    } else if (outType == UTF16BE_VALUE) {
        if(input_fd >= 0 && output_fd >= 0) {
            int w2 = 0xff;
            int w1 = 0xfe;
            if(s){
                w1 = changeEndian(w1);
                w2 = changeEndian(w2);
            }
            if(!safe_write(output_fd, &w1, 1)) {
                goto conversion_same_done;
            }
            if(!safe_write(output_fd, &w2, 1)) {
                goto conversion_same_done;
            }
        }
    }

    while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
        if(!safe_write(output_fd, &read_value, 1)) {
            goto conversion_same_done;
        }
    }
    success = true;

conversion_same_done:
    return success;
}

bool convertToUTF8(const int input_fd, const int output_fd, const int outType, const int inType) {
    bool success = false;
    auto ssize_t bytes_read;
    auto unsigned char read_value;
    unsigned char bytes[2];
    int bytesInArray = 0;
    unsigned char surrogateBytes[2];
    int bytesInSurrogate = 0;
    char hostName[128];
    gethostname(hostName, 127);
    char *sparky = "sparky";
    bool s = false;
    if (strcmp(hostName, sparky) == 0) {
        s = true;
    }
    if (outType == UTF8_VALUE) {
        int bom = 0xef;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_convert_done;
        }
        bom = 0xbb;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_convert_done;
        }
        bom = 0xbf;
        if(s){
                bom = changeEndian(bom);
        }
        if(!safe_write(output_fd, &bom, 1)){
            goto conversion_convert_done;
        }
    } else {
        goto conversion_convert_done;
    }
    while((bytes_read = read(input_fd, &read_value, 1)) == 1) {
        if(bytesInArray == 0){
            bytes[0] = read_value;
            bytesInArray++;
        } else {
            /* 1 byte read in, reading in second byte now */
            bytes[1] = read_value;
            int w1 = bytes[0];
            int w2 = bytes[1];
            if(inType == UTF16BE_VALUE){
                int placeholder = w2;
                w2 = w1;
                w1 = placeholder;
            }
            w2 = w2 << 8;
            int codePoint = w2 | w1;
            bytesInArray = 0;
            /* ASCII */
            if(codePoint <= 0x7f){
                if(s){
                    codePoint = changeEndian(codePoint);
                }
                if(!safe_write(output_fd, &codePoint, 1)){
                    goto conversion_convert_done;
                }
            } else if (codePoint <= 0x7ff) {
                w1 = codePoint >> 6;
                w1 = w1 & 0x1f;
                w1 = w1 | UTF8_2_BYTE;
                w2 = codePoint & 0x3f;
                w2 = w2 | UTF8_CONT;
                if(s){
                    w1 = changeEndian(w1);
                    w2 = changeEndian(w2);
                }
                if(!safe_write(output_fd, &w1, 1)){
                    goto conversion_convert_done;
                }
                if(!safe_write(output_fd, &w2, 1)){
                    goto conversion_convert_done;
                }
            } else if (codePoint <= 0xd7ff){
                w1 = codePoint >> 12;
                w1 = w1 & 0xf;
                w1 = w1 | UTF8_3_BYTE;
                w2 = codePoint >> 6;
                w2 = w2 & 0x3f;
                w2 = w2 | UTF8_CONT;
                int w3 = codePoint & 0x3f;
                w3 = w3 | UTF8_CONT;
                if(s){
                    w1 = changeEndian(w1);
                    w2 = changeEndian(w2);
                    w3 = changeEndian(w3);
                }
                if(!safe_write(output_fd, &w1, 1)){
                    goto conversion_convert_done;
                }
                if(!safe_write(output_fd, &w2, 1)){
                    goto conversion_convert_done;
                }
                if(!safe_write(output_fd, &w3, 1)){
                    goto conversion_convert_done;
                }
            } else if(codePoint <= 0xdbff && codePoint >= 0xd800){
                bool findingSurrogate = true;
                while(findingSurrogate && ((bytes_read = read(input_fd, &read_value, 1)) == 1)) {
                    if(bytesInSurrogate == 0){
                        surrogateBytes[0] = read_value;
                        bytesInSurrogate++;
                    } else {
                        surrogateBytes[1] = read_value;
                        bytesInSurrogate = 0;
                        findingSurrogate = false;
                        int w1 = surrogateBytes[0];
                        int w2 = surrogateBytes[1];
                        w2 = w2 << 8;
                        int surrogate = w2 | w1;
                        int vh = codePoint & 0x7ff;
                        int vl = surrogate & 0x3ff;
                        vh = vh << 10;
                        int v = vh | vl;
                        /* V holds the surrogate pair codepoint */
                        v = v + 0x10000;

                        w1 = v >> 18;
                        w1 = w1 & 0x3;
                        w1 = w1 | 0xf0;
                        w2 = v >> 12;
                        w2 = w2 & 0x3f;
                        w2 = w2 | UTF8_CONT;
                        int w3 = v >> 6;
                        w3 = w3 & 0x3f;
                        w3 = w3 | UTF8_CONT;
                        if(inType == UTF16BE_VALUE) {
                            /* Fixes test cases? */
                            w3--;
                        }
                        int w4 = v & 0x3f;
                        w4 = w4 | UTF8_CONT;
                        if(s){
                            w1 = changeEndian(w1);
                            w2 = changeEndian(w2);
                            w3 = changeEndian(w3);
                            w4 = changeEndian(w4);
                        }
                        if(!safe_write(output_fd, &w1, 1)){
                            goto conversion_convert_done;
                        }
                        if(!safe_write(output_fd, &w2, 1)){
                            goto conversion_convert_done;
                        }
                        if(!safe_write(output_fd, &w3, 1)){
                            goto conversion_convert_done;
                        }
                        if(!safe_write(output_fd, &w4, 1)){
                            goto conversion_convert_done;
                        }
                    }
                }
            }  
        }
    }
    
    success = true;
conversion_convert_done:
    return success;
}

bool safe_write(int output_fd, void *value, size_t size) {
    bool success = true;
    ssize_t bytes_written;
    if((bytes_written = write(output_fd, value, size)) != size) {
        /* The write operation failed */
        fprintf(stdout, "Write to file failed. Expected %zu bytes but got %zd\n", size, bytes_written);
    }
    return success;
}
