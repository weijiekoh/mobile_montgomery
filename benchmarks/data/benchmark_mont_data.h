typedef struct {
    int   cost;
    char* a_hex;
    char* b_hex;
    char* result_hex;
} BenchmarkData;

const int get_benchmark_data_length() {
    return 11;
}

const BenchmarkData* get_benchmark_data() {
    static const BenchmarkData dataArray[] = {
        {
            1024,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "288d8f838d8575326389c5fbec8452ba2c451ba01572146001762fd2e41546ea"
        },

        {
            2048,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "077ae088a17bd0fdf92315146022aa88a30c7f97fae2a9c8dfb86a3625db18fa"
        },

        {
            4096,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "2d071f5c357ad4a827d65d6573bb79ae5dd48bcc48594610902d6e6b6fa6c17c"
        },

        {
            8192,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "212772a60844b9bf97b52ac233d148490da84ce1187c2ca0ce658d2e52c17f9f"
        },

        {
            16384,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "169fccb5095e52c97a9552d93d293d4d4112ec44a2b903f2c4fb027e34346fa4"
        },

        {
            32768,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "2742c3f9ae871b6d13cf0c871ee8f91638e54cfc4897a3a8c4a1e1e73af1d931"
        },

        {
            65536,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "304da355935872f6ffa04e432422e8f08795c18beea99390bae5c6db21aa6cd6"
        },

        {
            131072,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "224f1eaf94f47bfc1de83cfd6f92e24a9652c71a99bc22053d9945cc4a17ac6b"
        },

        {
            262144,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "1a06c44563d1547a51dced2b645f28ae754c625deac613794e54d01724f339a7"
        },

        {
            524288,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "1a39761be445f7230c8e7d202a550a0f1041e51cb87580ee04bdbe04482c4176"
        },

        {
            1048576,
            "14a9c2762b8ab0f20cb1096618a19a05d483d5405f405ef524524a41d90fff2f",
            "0aefa8fa0094edcbcd47dd061763108702bbdc704174a53b54507c8c28c69c77",
            "301984cbee64ca3f26f65bc3625b111bb45ac0f8331440afc7ed2ed7d5442156"
        }

    };
    return dataArray;
}
