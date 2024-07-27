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

// Create Auth message
/**
 * @brief Initializes an Auth message with the provided username and password.
 *
 * @param id The username to set in the message.
 * @param pass The password to set in the message.
 * @param msg The Auth message to initialize.
 */

void create_auth(const char *id, const char *pass, Auth &msg) {
  msg = Auth_init_zero;
  if (id != NULL) {
    msg.id.funcs.encode = &encode_string;
    msg.id.arg = (void *)id;
  }
  if (pass != NULL) {
    msg.pass.funcs.encode = &encode_string;
    msg.pass.arg = (void *)pass;
  }
}

// Free memory for Auth message
/**
 * @brief Frees memory allocated for an Auth message.
 *
 * @param msg The Auth message to free.
 */
void free_auth(Auth &msg) {
  if (msg.id.arg) {
    free(msg.id.arg);
    msg.id.arg = NULL;
  }
  if (msg.pass.arg) {
    free(msg.pass.arg);
    msg.pass.arg = NULL;
  }
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

// Free memory for Strnum message
/**
 * @brief Frees memory allocated for a Strnum message.
 *
 * @param msg The Strnum message to free.
 */
void free_strnum(Strnum &msg) {
  if (msg.str.arg) {
    free(msg.str.arg);
    msg.str.arg = NULL;
  }
}
