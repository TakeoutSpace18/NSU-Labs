#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash_map.h"

#define TEST_STRINGS_COUNT 50

static const struct {
    const char *const key;
    const char *const value;
} testStrings[] = {
    {"first_name", "Emily"},
    {"last_name", "Smith"},
    {"age", "32"},
    {"email", "emily.smith@example.com"},
    {"phone_number", "+1-555-123-4567"},
    {"address", "123 Main St."},
    {"city", "San Francisco"},
    {"state", "CA"},
    {"zip_code", "94111"},
    {"country", "USA"},
    {"favorite_color", "blue"},
    {"hobby", "gardening"},
    {"food", "pizza"},
    {"animal", "dog"},
    {"movie_genre", "action"},
    {"book_genre", "mystery"},
    {"car_model", "Toyota Camry"},
    {"shoe_size", "8.5"},
    {"shirt_size", "medium"},
    {"hat_size", "7 1/4"},
    {"occupation", "teacher"},
    {"salary", "$50,000"},
    {"job_title", "manager"},
    {"company_name", "Acme Inc."},
    {"department", "sales"},
    {"product_name", "widget"},
    {"quantity", "10"},
    {"price", "$9.99"},
    {"order_date", "2023-05-14"},
    {"delivery_address", "456 Oak St."},
    {"delivery_city", "Los Angeles"},
    {"delivery_state", "CA"},
    {"delivery_zip_code", "90001"},
    {"payment_method", "credit card"},
    {"card_number", "1234-5678-9012-3456"},
    {"expiration_date", "12/25"},
    {"security_code", "123"},
    {"customer_id", "1001"},
    {"employee_id", "2002"},
    {"supplier_id", "3003"},
    {"vendor_id", "4004"},
    {"product_id", "5005"},
    {"transaction_id", "6006"},
    {"invoice_number", "7007"},
    {"quote_reference", "8008"},
    {"reservation_number", "9009"},
    {"tracking_number", "ABC123"},
    {"serial_number", "XYZ789"},
    {"model_number", "123-456"},
    {"unique_id", "a1b2c3d4e5"}
};

static const struct {
    const char *const key;
    const char *const value;
} replaceTest[] = {
    {"foo", "123"},
    {"foo", "456"},
    {"foo", "789"},
};


int main(void) {
    HashMap_t *hashMap = hm_create();

    printf("\t     $$$ ADDING $$$\n");
    for (size_t i = 0; i < TEST_STRINGS_COUNT; ++i) {
        if (i % 10 == 0) {
            hm_printStatistics(stdout, hashMap);
        }

        const char *key = testStrings[i].key;
        const char *value = testStrings[i].value;

        size_t key_len = strlen(key);

        hm_put(hashMap, key, key_len, value);
    }

    printf("\n\n\t    $$$ REMOVING $$$\n");
    for (size_t i = 0; i < TEST_STRINGS_COUNT; ++i) {
        if (i % 10 == 0) {
            hm_printStatistics(stdout, hashMap);
        }

        const char *key = testStrings[i].key;
        size_t key_len = strlen(key);

        char *value = hm_remove(hashMap, key, key_len);
        if (strcmp(value, testStrings[i].value) == 0) {
            printf("{%s, %s} removed.\n", key, value);
        }
        else {
            printf("Value doesn't match!\n");
            return EXIT_FAILURE;
        }
    }

    hm_printStatistics(stdout, hashMap);
    printf("\n\n\t $$$ REPLACE TEST $$$\n");
    for (size_t i = 0; i < 3; ++i) {
        const char *key = replaceTest[i].key;
        const char *value = replaceTest[i].value;

        size_t key_len = strlen(key);

        hm_put(hashMap, key, key_len, value);

        const char *get_value = hm_get(hashMap, key, key_len);
        printf("%zu) {%s, %s} in hash map.\n", i, key, get_value);
    }
    hm_printStatistics(stdout, hashMap);

    hm_destroy(&hashMap);
    return EXIT_SUCCESS;
}
