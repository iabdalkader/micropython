/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Arduino SA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "py/mpconfig.h"

#if MICROPY_PY_CRYPTOKI && MICROPY_HW_ENABLE_SE05X

// This module implements a subset of the PCKS#11 Cryptoki API.
#include <string.h>
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#if MICROPY_HW_ENABLE_SE05X
#include "sm_port.h"
#include "sm_i2c.h"
#include "sm_timer.h"
#include "se05x_APDU_apis.h"
typedef Se05xSession_t token_ctx_t;
#endif

typedef struct _mp_obj_token_t {
    mp_obj_base_t base;
    token_ctx_t ctx;
} mp_token_obj_t;

STATIC const mp_obj_type_t cryptoki_token_type;

STATIC mp_obj_t cryptoki_token_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 3, false);

    mp_token_obj_t *t = m_new_obj_with_finaliser(mp_token_obj_t);
    t->base.type = &cryptoki_token_type;
    memset(&t->ctx, 0, sizeof(t->ctx));
    #if MICROPY_HW_ENABLE_SE05X
    Se05x_API_SessionOpen(&t->ctx);
    #endif
    return MP_OBJ_FROM_PTR(t);
}

mp_obj_t cryptoki_token_close(mp_obj_t token) {
    mp_token_obj_t *self = MP_OBJ_TO_PTR(token);
    #if MICROPY_HW_ENABLE_SE05X
    Se05x_API_SessionClose(&self->ctx);
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptoki_token_close_obj, cryptoki_token_close);

STATIC mp_obj_t cryptoki_token_exit(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return cryptoki_token_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cryptoki_token_exit_obj, 4, 4, cryptoki_token_exit);

mp_obj_t cryptoki_token_info(mp_obj_t token) {
    mp_token_obj_t *self = MP_OBJ_TO_PTR(token);
    #if MICROPY_HW_ENABLE_SE05X
    uint8_t version[64] = {0};
    size_t version_len = sizeof(version);
    Se05x_API_GetVersion(&self->ctx, version, &version_len);
    return mp_obj_new_tuple(3, (mp_obj_t []) {
        mp_obj_new_int(version[0]),
        mp_obj_new_int(version[1]),
        mp_obj_new_int(version[2])
    });
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(cryptoki_token_info_obj, cryptoki_token_info);

STATIC mp_obj_t cryptoki_token_read(size_t n_args, const mp_obj_t *args) {
    vstr_t vstr;
    mp_token_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t id = mp_obj_get_int(args[1]);
    vstr_init_len(&vstr, mp_obj_get_int(args[2]));
    #if MICROPY_HW_ENABLE_SE05X
    SE05x_Result_t result;
    if (Se05x_API_CheckObjectExists(&self->ctx, id, &result) != SM_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Error in Se05x_API_CheckObjectExists"));
    }
    if (result != kSE05x_Result_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("Object doesn't exist"));
    }
    size_t blk_size = 128;  // MAX xfer size.
    size_t blk_count = vstr.len / blk_size;
    size_t offset = 0;
    for (mp_int_t i=0; i<blk_count; i++, offset += blk_size) {
        size_t lenout = blk_size;
        if (Se05x_API_ReadObject(&self->ctx, id, offset, blk_size, (uint8_t *) vstr.buf + offset, &lenout) != SM_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Error in Se05x_API_ReadObject"));
        }
    }
    if (offset < vstr.len) {
        size_t remain = vstr.len - offset;
        if (Se05x_API_ReadObject(&self->ctx, id, offset, remain, (uint8_t *) vstr.buf + offset, &remain) != SM_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Error in Se05x_API_ReadObject"));
        }
    }
    #endif
    return mp_obj_new_bytes_from_vstr(&vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cryptoki_token_read_obj, 2, 3, cryptoki_token_read);

STATIC const mp_rom_map_elem_t cryptoki_token_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_close),       MP_ROM_PTR(&cryptoki_token_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_info),        MP_ROM_PTR(&cryptoki_token_info_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&cryptoki_token_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),   MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),    MP_ROM_PTR(&cryptoki_token_exit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),    MP_ROM_PTR(&cryptoki_token_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(cryptoki_token_locals_dict, cryptoki_token_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    cryptoki_token_type,
    MP_QSTR_token,
    MP_TYPE_FLAG_NONE,
    make_new, cryptoki_token_make_new,
    locals_dict, &cryptoki_token_locals_dict
    );

STATIC const mp_rom_map_elem_t mp_module_cryptoki_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cryptoki) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&cryptoki_token_type) },
    //{ MP_ROM_QSTR(MP_QSTR_), MP_ROM_INT(MOD_CRYPTOKI_) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_cryptoki_globals, mp_module_cryptoki_globals_table);

const mp_obj_module_t mp_module_cryptoki = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_cryptoki_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptoki, mp_module_cryptoki);

#endif // MICROPY_PY_CRYPTOKI
