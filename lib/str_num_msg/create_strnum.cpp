#include "./str_num_msg_transcode.h"
#include <stdlib.h>

/**
 * @brief Initializes a Num message with the provided values.
 *
 * @param number The float value to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Num message to initialize.
 */
void create_num(float number, uint32_t key, Num &msg) {
  msg = Num_init_zero;
  msg.key = key;
  msg.value = number;
}



// Create Str message
/**
 * @brief Initializes a Str message with the provided string and key.
 *
 * @param str The string to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Str message to initialize.
 */
void create_str(const char *str, uint32_t key, Str &msg) {
  msg = Str_init_zero;
  msg.key = key;
  if (str != NULL) {
    msg.value.funcs.encode = &encode_string;
    msg.value.arg = (char *)str;
  }
}

// Create Strnum message
/**
 * @brief Initializes a Strnum message with the provided string, number, and
 * key.
 *
 * @param str The string to set in the message.
 * @param num The float value to set in the message.
 * @param key The key to set in the message.
 * @param msg Pointer to the Strnum message to initialize.
 */
void create_strnum(const char *str, float num, uint32_t key, Strnum &msg) {
  msg = Strnum_init_zero;
  msg.key = key;
  msg.num = num;
  if (str != NULL) {
    msg.str.funcs.encode = &encode_string;
    msg.str.arg = (void *)str;
  }
}


void create_stringlist(const char *str[], size_t num_strs, Stringlist &msg) {
  msg = Stringlist_init_zero;
  for(size_t i = 0; i < num_strs; i++) {
    msg.strs.funcs.encode = &encode_string;
    msg.strs.arg = (void *)str[i];
  }
}

void create_strnumlist(const Strnum *strum, Strnumlist &msg) {
  msg = Strnumlist_init_zero;
  msg.strnums.funcs.encode = &pb_encode_strnum;
  msg.strnums.arg = (void *)strum;
}

// Free memory for Str message
/**
 * @brief Frees memory allocated for a Str message.
 *
 * @param msg The Str message to free.
 */
void free_str(Str &msg) {
  if (msg.value.arg) {
    free(msg.value.arg);
    msg.value.arg = NULL;
  }
}

void free_stringlist(Stringlist &msg) {
  if (msg.strs.arg) {
    free(msg.strs.arg);
    msg.strs.arg = NULL;
  }
}

void free_strnumlist(Strnumlist &msg) {
  if (msg.strnums.arg) {
    free(msg.strnums.arg);
    msg.strnums.arg = NULL;
  }
}


// Create Strnumlist message
/**
 * @brief Initializes a Strnumlist message with the provided Strnum data.
 *
 * @param strum The Strnum data to set in the message.
 * @param msg Pointer to the Strnumlist message to initialize.
 */
void create_strnumlst(const Strnum *strum, Strnumlist &msg) {
  msg = Strnumlist_init_zero;
  msg.strnums.funcs.encode = &pb_encode_strnum;
  msg.strnums.arg = (void *)strum;
}
