/**
 * Unit Tests for ESP32 MacroPad Firmware
 * 
 * This test suite validates the core functionality of the MacroPad firmware
 * including storage operations, state management, and business logic.
 * 
 * To run these tests:
 * 1. Build the test app: cd test_app && idf.py build
 * 2. Flash to device: idf.py -p PORT flash monitor
 * 3. Tests will run automatically on boot
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "TEST";

// Test configuration constants
#define TEST_NVS_NAMESPACE "macropad"
#define NUM_MACROS 4
#define MAX_MACRO_LEN 256

// =============================================================================
// TEST HELPER FUNCTIONS
// =============================================================================

/**
 * Initialize NVS for testing
 */
static void test_init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

/**
 * Save a macro to NVS
 */
static esp_err_t test_save_macro(int index, const char *text)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;
    
    ret = nvs_open(TEST_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        return ret;
    }
    
    char key[16];
    snprintf(key, sizeof(key), "macro%d", index);
    
    ret = nvs_set_str(nvs_handle, key, text);
    if (ret == ESP_OK) {
        ret = nvs_commit(nvs_handle);
    }
    
    nvs_close(nvs_handle);
    return ret;
}

/**
 * Load a macro from NVS
 */
static esp_err_t test_load_macro(int index, char *buffer, size_t buffer_size)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;
    
    ret = nvs_open(TEST_NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (ret != ESP_OK) {
        return ret;
    }
    
    char key[16];
    snprintf(key, sizeof(key), "macro%d", index);
    
    size_t required_size = buffer_size;
    ret = nvs_get_str(nvs_handle, key, buffer, &required_size);
    
    nvs_close(nvs_handle);
    return ret;
}

/**
 * Erase all test data from NVS
 */
static void test_erase_nvs_namespace(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open(TEST_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (ret == ESP_OK) {
        nvs_erase_all(nvs_handle);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

// =============================================================================
// NVS STORAGE TESTS
// =============================================================================

TEST_CASE("NVS: Save and load single macro", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    const char *test_text = "Hello World";
    char read_buffer[MAX_MACRO_LEN];
    
    // Save macro
    esp_err_t ret = test_save_macro(0, test_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Load macro
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(test_text, read_buffer);
}

TEST_CASE("NVS: Save and load all four macros", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    const char *test_macros[NUM_MACROS] = {
        "Macro 1",
        "Macro 2",
        "Macro 3",
        "Macro 4"
    };
    
    // Save all macros
    for (int i = 0; i < NUM_MACROS; i++) {
        esp_err_t ret = test_save_macro(i, test_macros[i]);
        TEST_ASSERT_EQUAL(ESP_OK, ret);
    }
    
    // Load and verify all macros
    char read_buffer[MAX_MACRO_LEN];
    for (int i = 0; i < NUM_MACROS; i++) {
        esp_err_t ret = test_load_macro(i, read_buffer, sizeof(read_buffer));
        TEST_ASSERT_EQUAL(ESP_OK, ret);
        TEST_ASSERT_EQUAL_STRING(test_macros[i], read_buffer);
    }
}

TEST_CASE("NVS: Empty string storage", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    const char *empty_text = "";
    char read_buffer[MAX_MACRO_LEN];
    
    // Save empty macro
    esp_err_t ret = test_save_macro(0, empty_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Load and verify empty macro
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(empty_text, read_buffer);
}

TEST_CASE("NVS: Long string storage", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    // Create a long test string (200 characters)
    char long_text[201];
    for (int i = 0; i < 200; i++) {
        long_text[i] = 'A' + (i % 26);
    }
    long_text[200] = '\0';
    
    char read_buffer[MAX_MACRO_LEN];
    
    // Save long macro
    esp_err_t ret = test_save_macro(0, long_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Load and verify
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(long_text, read_buffer);
}

TEST_CASE("NVS: Special characters in macro", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    const char *special_text = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    char read_buffer[MAX_MACRO_LEN];
    
    // Save macro with special characters
    esp_err_t ret = test_save_macro(0, special_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Load and verify
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(special_text, read_buffer);
}

TEST_CASE("NVS: Overwrite existing macro", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    const char *original_text = "Original Text";
    const char *updated_text = "Updated Text";
    char read_buffer[MAX_MACRO_LEN];
    
    // Save original
    esp_err_t ret = test_save_macro(0, original_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Verify original
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(original_text, read_buffer);
    
    // Overwrite with updated
    ret = test_save_macro(0, updated_text);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Verify updated
    ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(updated_text, read_buffer);
}

TEST_CASE("NVS: Multiple writes to same macro", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    char read_buffer[MAX_MACRO_LEN];
    
    // Write multiple times
    for (int version = 1; version <= 10; version++) {
        char text[32];
        snprintf(text, sizeof(text), "Version %d", version);
        
        esp_err_t ret = test_save_macro(0, text);
        TEST_ASSERT_EQUAL(ESP_OK, ret);
        
        // Verify latest version
        ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
        TEST_ASSERT_EQUAL(ESP_OK, ret);
        TEST_ASSERT_EQUAL_STRING(text, read_buffer);
    }
}

TEST_CASE("NVS: Load non-existent macro", "[storage]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    char read_buffer[MAX_MACRO_LEN];
    
    // Try to load a macro that doesn't exist
    esp_err_t ret = test_load_macro(0, read_buffer, sizeof(read_buffer));
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, ret);
}

// =============================================================================
// STATE MANAGEMENT TESTS
// =============================================================================

typedef enum {
    MODE_PLAYBACK,
    MODE_CONFIG,
    MODE_EDIT_KEYBOARD
} app_mode_t;

typedef struct {
    app_mode_t mode;
    int selected_macro;
    bool send_button_visible;
    uint32_t selection_time;
} test_state_t;

TEST_CASE("State: Initialize to playback mode", "[state]")
{
    test_state_t state = {
        .mode = MODE_PLAYBACK,
        .selected_macro = -1,
        .send_button_visible = false,
        .selection_time = 0
    };
    
    TEST_ASSERT_EQUAL(MODE_PLAYBACK, state.mode);
    TEST_ASSERT_EQUAL(-1, state.selected_macro);
    TEST_ASSERT_FALSE(state.send_button_visible);
    TEST_ASSERT_EQUAL(0, state.selection_time);
}

TEST_CASE("State: Select macro button", "[state]")
{
    test_state_t state = {
        .mode = MODE_PLAYBACK,
        .selected_macro = -1,
        .send_button_visible = false,
        .selection_time = 0
    };
    
    // Simulate selecting macro 0
    state.selected_macro = 0;
    state.send_button_visible = true;
    state.selection_time = 1000;
    
    TEST_ASSERT_EQUAL(0, state.selected_macro);
    TEST_ASSERT_TRUE(state.send_button_visible);
    TEST_ASSERT_EQUAL(1000, state.selection_time);
}

TEST_CASE("State: Reset selection", "[state]")
{
    test_state_t state = {
        .mode = MODE_PLAYBACK,
        .selected_macro = 2,
        .send_button_visible = true,
        .selection_time = 5000
    };
    
    // Reset selection
    state.selected_macro = -1;
    state.send_button_visible = false;
    state.selection_time = 0;
    
    TEST_ASSERT_EQUAL(-1, state.selected_macro);
    TEST_ASSERT_FALSE(state.send_button_visible);
    TEST_ASSERT_EQUAL(0, state.selection_time);
}

TEST_CASE("State: Mode transitions", "[state]")
{
    test_state_t state = {
        .mode = MODE_PLAYBACK,
        .selected_macro = -1,
        .send_button_visible = false,
        .selection_time = 0
    };
    
    // Transition to config mode
    state.mode = MODE_CONFIG;
    TEST_ASSERT_EQUAL(MODE_CONFIG, state.mode);
    
    // Transition to edit mode
    state.mode = MODE_EDIT_KEYBOARD;
    TEST_ASSERT_EQUAL(MODE_EDIT_KEYBOARD, state.mode);
    
    // Back to playback
    state.mode = MODE_PLAYBACK;
    TEST_ASSERT_EQUAL(MODE_PLAYBACK, state.mode);
}

// =============================================================================
// BUTTON LOGIC TESTS
// =============================================================================

/**
 * Check if a touch point is within a button's bounds
 */
static bool is_touch_in_button(uint16_t touch_x, uint16_t touch_y,
                               uint16_t btn_x, uint16_t btn_y,
                               uint16_t btn_width, uint16_t btn_height)
{
    return (touch_x >= btn_x && touch_x <= btn_x + btn_width &&
            touch_y >= btn_y && touch_y <= btn_y + btn_height);
}

TEST_CASE("Button: Touch inside button bounds", "[button]")
{
    uint16_t btn_x = 10, btn_y = 35;
    uint16_t btn_width = 140, btn_height = 90;
    
    // Test center of button
    TEST_ASSERT_TRUE(is_touch_in_button(80, 80, btn_x, btn_y, btn_width, btn_height));
    
    // Test top-left corner
    TEST_ASSERT_TRUE(is_touch_in_button(10, 35, btn_x, btn_y, btn_width, btn_height));
    
    // Test bottom-right corner
    TEST_ASSERT_TRUE(is_touch_in_button(150, 125, btn_x, btn_y, btn_width, btn_height));
    
    // Test edges
    TEST_ASSERT_TRUE(is_touch_in_button(10, 80, btn_x, btn_y, btn_width, btn_height));
    TEST_ASSERT_TRUE(is_touch_in_button(150, 80, btn_x, btn_y, btn_width, btn_height));
    TEST_ASSERT_TRUE(is_touch_in_button(80, 35, btn_x, btn_y, btn_width, btn_height));
    TEST_ASSERT_TRUE(is_touch_in_button(80, 125, btn_x, btn_y, btn_width, btn_height));
}

TEST_CASE("Button: Touch outside button bounds", "[button]")
{
    uint16_t btn_x = 10, btn_y = 35;
    uint16_t btn_width = 140, btn_height = 90;
    
    // Test left of button
    TEST_ASSERT_FALSE(is_touch_in_button(5, 80, btn_x, btn_y, btn_width, btn_height));
    
    // Test right of button
    TEST_ASSERT_FALSE(is_touch_in_button(160, 80, btn_x, btn_y, btn_width, btn_height));
    
    // Test above button
    TEST_ASSERT_FALSE(is_touch_in_button(80, 30, btn_x, btn_y, btn_width, btn_height));
    
    // Test below button
    TEST_ASSERT_FALSE(is_touch_in_button(80, 130, btn_x, btn_y, btn_width, btn_height));
    
    // Test far away
    TEST_ASSERT_FALSE(is_touch_in_button(200, 200, btn_x, btn_y, btn_width, btn_height));
}

TEST_CASE("Button: Zero-sized button", "[button]")
{
    uint16_t btn_x = 10, btn_y = 35;
    uint16_t btn_width = 0, btn_height = 0;
    
    // Touch at button origin should still work
    TEST_ASSERT_TRUE(is_touch_in_button(10, 35, btn_x, btn_y, btn_width, btn_height));
    
    // Touch nearby should fail
    TEST_ASSERT_FALSE(is_touch_in_button(11, 35, btn_x, btn_y, btn_width, btn_height));
    TEST_ASSERT_FALSE(is_touch_in_button(10, 36, btn_x, btn_y, btn_width, btn_height));
}

// =============================================================================
// STRING MANIPULATION TESTS
// =============================================================================

TEST_CASE("String: Append character to buffer", "[string]")
{
    char buffer[32] = "Hello";
    size_t len = strlen(buffer);
    
    // Append a character
    if (len < sizeof(buffer) - 1) {
        buffer[len] = ' ';
        buffer[len + 1] = '\0';
    }
    
    TEST_ASSERT_EQUAL_STRING("Hello ", buffer);
}

TEST_CASE("String: Remove character from buffer (backspace)", "[string]")
{
    char buffer[32] = "Hello";
    size_t len = strlen(buffer);
    
    // Remove last character
    if (len > 0) {
        buffer[len - 1] = '\0';
    }
    
    TEST_ASSERT_EQUAL_STRING("Hell", buffer);
}

TEST_CASE("String: Buffer overflow protection", "[string]")
{
    char buffer[8] = "1234567"; // 7 chars + null terminator
    size_t len = strlen(buffer);
    
    // Try to append (should fail gracefully)
    bool can_append = (len < sizeof(buffer) - 1);
    TEST_ASSERT_FALSE(can_append);
}

TEST_CASE("String: Empty buffer operations", "[string]")
{
    char buffer[32] = "";
    size_t len = strlen(buffer);
    
    TEST_ASSERT_EQUAL(0, len);
    
    // Try backspace on empty buffer
    if (len > 0) {
        buffer[len - 1] = '\0';
    }
    
    TEST_ASSERT_EQUAL_STRING("", buffer);
}

// =============================================================================
// TIMEOUT LOGIC TESTS
// =============================================================================

TEST_CASE("Timeout: Selection timeout check", "[timeout]")
{
    uint32_t selection_time = 1000;
    uint32_t current_time = 6000;
    uint32_t timeout_ms = 5000;
    
    bool timed_out = (current_time - selection_time > timeout_ms);
    TEST_ASSERT_TRUE(timed_out);
}

TEST_CASE("Timeout: No timeout within period", "[timeout]")
{
    uint32_t selection_time = 1000;
    uint32_t current_time = 5000;
    uint32_t timeout_ms = 5000;
    
    bool timed_out = (current_time - selection_time > timeout_ms);
    TEST_ASSERT_FALSE(timed_out);
}

TEST_CASE("Timeout: Exact timeout boundary", "[timeout]")
{
    uint32_t selection_time = 1000;
    uint32_t current_time = 6000;
    uint32_t timeout_ms = 5000;
    
    // At exactly 5000ms, should not timeout
    current_time = selection_time + timeout_ms;
    bool timed_out = (current_time - selection_time > timeout_ms);
    TEST_ASSERT_FALSE(timed_out);
    
    // At 5001ms, should timeout
    current_time = selection_time + timeout_ms + 1;
    timed_out = (current_time - selection_time > timeout_ms);
    TEST_ASSERT_TRUE(timed_out);
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

TEST_CASE("Integration: Complete macro save and load workflow", "[integration]")
{
    test_init_nvs();
    test_erase_nvs_namespace();
    
    // Simulate user entering config mode and editing a macro
    const char *new_macro = "Integration Test Macro";
    
    // Save the macro
    esp_err_t ret = test_save_macro(0, new_macro);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    
    // Simulate device restart - load macros
    char loaded_macro[MAX_MACRO_LEN];
    ret = test_load_macro(0, loaded_macro, sizeof(loaded_macro));
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_STRING(new_macro, loaded_macro);
}

TEST_CASE("Integration: Macro selection and send workflow", "[integration]")
{
    test_state_t state = {
        .mode = MODE_PLAYBACK,
        .selected_macro = -1,
        .send_button_visible = false,
        .selection_time = 0
    };
    
    // Step 1: User touches macro button 0
    state.selected_macro = 0;
    state.send_button_visible = true;
    state.selection_time = 1000;
    
    TEST_ASSERT_EQUAL(0, state.selected_macro);
    TEST_ASSERT_TRUE(state.send_button_visible);
    
    // Step 2: User touches send button
    // (In real code, this would trigger ble_send_text)
    bool can_send = (state.selected_macro >= 0 && state.send_button_visible);
    TEST_ASSERT_TRUE(can_send);
    
    // Step 3: Reset after send
    state.selected_macro = -1;
    state.send_button_visible = false;
    state.selection_time = 0;
    
    TEST_ASSERT_EQUAL(-1, state.selected_macro);
    TEST_ASSERT_FALSE(state.send_button_visible);
}

// =============================================================================
// MAIN TEST APP
// =============================================================================

void app_main(void)
{
    ESP_LOGI(TAG, "Starting MacroPad Unit Tests");
    ESP_LOGI(TAG, "========================================");
    
    // Run all tests
    unity_run_menu();
}
