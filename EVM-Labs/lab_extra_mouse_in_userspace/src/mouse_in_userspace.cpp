#include <iostream>
#include <cstdio>
#include <cstring>
#include <libusb-1.0/libusb.h>

#define MOUSE_INTERFACE_NUM 1
#define MOUSE_ENDPOINT_ADDRESS 0x82

struct mouse_packet
{
    int8_t buttons;
    int16_t direction_x;
    int16_t direction_y;
    int8_t wheel;
} __attribute__((packed));

libusb_device* find_mouse(libusb_device** devices, int num_devices)
{
    for (int i = 0; i < num_devices; ++i)
    {
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(devices[i], &desc);
        if (desc.idProduct == 0x7cc6)
        {
            return devices[i];
        }
    }

    return nullptr;
}

void update_console_loop(libusb_device_handle* mouse_handle)
{
    int cursor_x = 1;
    int cursor_y = 1;

    while (true)
    {
        mouse_packet data;
        int actual_length;
        int err = libusb_interrupt_transfer(mouse_handle, MOUSE_ENDPOINT_ADDRESS,
                                        reinterpret_cast<unsigned char *>(&data), sizeof(data), &actual_length, 0);
        if (err)
        {
            std::cerr << "Error: failed to read mouse data. " << libusb_strerror(err) << std::endl;
        }

        if (data.buttons == 1) break;

        if (data.direction_x > 0) cursor_x += 1;
        if (data.direction_x < 0) cursor_x -= 1;
        if (data.direction_y > 0) cursor_y += 1;
        if (data.direction_y < 0) cursor_y -= 1;

        if (cursor_x < 0) cursor_x = 0;
        if (cursor_y < 0) cursor_y = 0;

        // Clear the screen
        std::cout << "\033[2J";

        // Move the cursor to the top of the screen
        std::cout << "\033[1;1H";

        // Print mouse data
        std::printf("%d\t%d\t%d\t%d\n", data.buttons, data.direction_x, data.direction_y, data.wheel);
        std::printf("%d\t%d\n", cursor_x, cursor_y);

        // Move to the cursor pos and print something
        std::printf("\033[%i;%iH", cursor_y, cursor_x);
        std::printf("OOO");
        std::printf("\033[%i;%iH", cursor_y+1, cursor_x);
        std::printf("OOO");

        std::fflush(stdout);
    }
}

int main()
{
    libusb_device** devices;
    libusb_context* ctx = nullptr;

    int err = libusb_init(&ctx);
    if (err < 0)
    {
        std::cerr << "Error: failed to initialize libusb. " << libusb_strerror(err) << std::endl;
        return EXIT_FAILURE;
    }

    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 3);

    ssize_t devices_count = libusb_get_device_list(ctx, &devices);
    if (devices_count < 0)
    {
        std::cerr << "Error: failed to get USB devices list. " << libusb_strerror(devices_count) << std::endl;
        return EXIT_FAILURE;
    }

    libusb_device* mouse = find_mouse(devices, devices_count);
    if (!mouse)
    {
        std::cerr << "Error: desired mouse not found" << std::endl;
    }

    libusb_device_handle* mouse_handle;
    libusb_open(mouse, &mouse_handle);

    libusb_set_auto_detach_kernel_driver(mouse_handle, true);
    err = libusb_claim_interface(mouse_handle, MOUSE_INTERFACE_NUM);
    if (err)
    {
        std::cerr << "Error: failed to claim device interface. " << libusb_strerror(err) << std::endl;
    }

    update_console_loop(mouse_handle);

    libusb_release_interface(mouse_handle, MOUSE_INTERFACE_NUM);
    libusb_close(mouse_handle);

    libusb_free_device_list(devices, 1);
    libusb_exit(ctx);
    return EXIT_SUCCESS;
}
