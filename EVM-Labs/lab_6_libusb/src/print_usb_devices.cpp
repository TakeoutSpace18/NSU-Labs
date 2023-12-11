#include <iostream>
#include <cstdio>
#include <cstring>
#include <libusb-1.0/libusb.h>
#include <libudev.h>

udev* udev_ctx = udev_new();
udev_hwdb* hwdb = udev_hwdb_new(udev_ctx);

void print_device_info(libusb_device * device);

static const char *hwdb_get(const char *modalias, const char *key) {
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, udev_hwdb_get_properties_list_entry(hwdb, modalias, 0))
        if (strcmp(udev_list_entry_get_name(entry), key) == 0)
            return udev_list_entry_get_value(entry);

    return NULL;
}

static const char *names_vendor(uint16_t vendorid)
{
    char modalias[64];

    sprintf(modalias, "usb:v%04X*", vendorid);
    return hwdb_get(modalias, "ID_VENDOR_FROM_DATABASE");
}

static const char *names_product(uint16_t vendorid, uint16_t productid)
{
    char modalias[64];

    sprintf(modalias, "usb:v%04Xp%04X*", vendorid, productid);
    return hwdb_get(modalias, "ID_MODEL_FROM_DATABASE");
}

static const char *names_class(uint8_t classid)
{
    char modalias[64];

    sprintf(modalias, "usb:v*p*d*dc%02X*", classid);
    return hwdb_get(modalias, "ID_USB_CLASS_FROM_DATABASE");
}

int main() {
    libusb_device** devices;
    libusb_context* ctx = NULL;

    int ret = libusb_init(&ctx);
    if (ret < 0) {
        std::cerr << "Error: failed to initialize libusb. Code: " << ret << std::endl;
        return EXIT_FAILURE;
    }

    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 3);

    ssize_t devices_count = libusb_get_device_list(ctx, &devices);
    if (devices_count < 0) {
        std::cerr << "Error: failed to get USB devices list. Code: " << ret << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Found devices: " << devices_count << std::endl;

    for (ssize_t i = 0; i < devices_count; i++) {
        std::cout << std::endl << "---------- DEVICE " << i+1 << "/" << devices_count << " ----------" << std::endl;
        print_device_info(devices[i]);
    }

    libusb_free_device_list(devices, 1);
    libusb_exit(ctx);
    hwdb = udev_hwdb_unref(hwdb);
    udev_ctx = udev_unref(udev_ctx);
    return EXIT_SUCCESS;
}

void print_device_info(libusb_device* device) {
    libusb_device_handle* dev_handle;
    int ret = libusb_open(device, &dev_handle);
    if (ret < 0) {
        std::cerr << "Error: failed to open device. Code: " << ret << std::endl;
        return;
    }

    libusb_device_descriptor device_descriptor;
    libusb_config_descriptor* config_descriptor;

    ret = libusb_get_device_descriptor(device, &device_descriptor);
    if (ret < 0) {
        std::cerr << "Error: failed to get device descriptor. Code: " << ret << std::endl;
        return;
    }

    libusb_get_config_descriptor(device, 0, &config_descriptor);

    unsigned char ascii_buffer[200];

    libusb_get_string_descriptor_ascii(dev_handle, device_descriptor.iManufacturer, ascii_buffer, 200);
    std::cout << "Manufacturer ascii: "<< ascii_buffer << std::endl;

    libusb_get_string_descriptor_ascii(dev_handle, device_descriptor.iProduct, ascii_buffer, 200);
    std::cout << "Product ascii: " << ascii_buffer << std::endl;

    libusb_get_string_descriptor_ascii(dev_handle, device_descriptor.iSerialNumber, ascii_buffer, 200);
    std::cout << "Serial number ascii: " << ascii_buffer << std::endl;

    libusb_get_string_descriptor_ascii(dev_handle, config_descriptor->iConfiguration, ascii_buffer, 200);
    std::cout << "Configuration ascii: " << ascii_buffer << std::endl;

    std::cout << "Configurations number: " << std::dec << (int)device_descriptor.bNumConfigurations << std::endl;



    uint16_t vendor_id = device_descriptor.idVendor;
    uint16_t class_id = device_descriptor.bDeviceClass;
    uint16_t product_id = device_descriptor.idProduct;

    const char* class_name = names_class(vendor_id);
    std::cout << "Device class: " << std::hex << (int)class_id;
    if (class_name) {
        std::cout << " (" << class_name << ")";
    }
    std::cout << std::endl;

    const char* vendor_name = names_vendor(vendor_id);
    std::cout << "Vendor id: " << std::hex << vendor_id;
    if (vendor_name) {
        std::cout << " (" << vendor_name << ")";
    }
    std::cout << std::endl;

    const char* product_name = names_product(vendor_id, product_id);
    std::cout << "Product id: " << std::hex << product_id;
    if (product_name) {
        std::cout << " (" << product_name << ")";
    }
    std::cout << std::endl;

    std::cout << "Interfaces number: " << std::dec << (int)config_descriptor->bNumInterfaces << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < (int)config_descriptor->bNumInterfaces; i++) {
        const libusb_interface* inter = &config_descriptor->interface[i];
        std::cout << "Interface #" << i << std::endl;
        std::cout << "- Alternative settings count: " << std::dec << inter->num_altsetting << std::endl;
        std::cout << "- Device class: " << std::hex << (int)device_descriptor.bDeviceClass << std::endl;

        for (int j = 0; j < inter->num_altsetting; j++) {
            const libusb_interface_descriptor* interface_descriptor = &inter->altsetting[j];

            std::cout << "- Altsetting #" << j << std::endl;

            libusb_get_string_descriptor_ascii(dev_handle, interface_descriptor->iInterface, ascii_buffer, 200);
            std::cout << "--  Interface ascii: " << ascii_buffer << std::endl;

            std::cout << "--  Interface number: " << std::dec << (int)interface_descriptor->bInterfaceNumber << std::endl;
            std::cout << "--  Endpoints count: " << std::dec << (int)interface_descriptor->bNumEndpoints << std::endl;

            for (int k = 0; k < (int)interface_descriptor->bNumEndpoints; k++) {
                const libusb_endpoint_descriptor* endpoint_descriptor = &interface_descriptor->endpoint[k];

                std::cout << "--  Endpoint #" << k << std::endl;
                std::cout << "---   Descriptor type: " << std::dec << (int)endpoint_descriptor->bDescriptorType << std::endl;
                std::cout << "---   Endpoint address: " << std::dec << (int)endpoint_descriptor->bEndpointAddress << std::endl;

            }
        }
    }

    libusb_close(dev_handle);
    libusb_free_config_descriptor(config_descriptor);
}
