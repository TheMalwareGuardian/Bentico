#include <windows.h>
#include <stdio.h>
#include <string.h>


#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)


int main()
{
    HANDLE hDevice;
    DWORD bytesReturned;
    BOOL success;
    int option;
    char inbuffer[15] = { 0 };
    char outbuffer[15] = { 0 };

    hDevice = CreateFile(L"\\\\.\\MyKernelDriver", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("Error: Failed to access the device (MyKernelDriver). Please make sure the driver is installed.\n");
        return 1;
    }

    while (1)
    {
        printf("\nMenu:\n");
        printf("0. Send IOCTL_COMMAND_0\n");
        printf("1. Send IOCTL_COMMAND_1\n");
        printf("2. Send IOCTL_COMMAND_2\n");
        printf("3. Exit\n");
        printf("Select an option: ");
        scanf_s("%d", &option);

        switch (option)
        {

            case 0:
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_0, NULL, 0, NULL, 0, &bytesReturned, NULL);
                if (success)
                {
                    printf("IOCTL_COMMAND_0 sent successfully\n");
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_0\n");
                }
                break;

            case 1:
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_1, NULL, 0, outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
                if (success)
                {
                    printf("IOCTL_COMMAND_1 sent successfully\n");
                    printf("Message received from kernel driver: %s\n", outbuffer);
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_1\n");
                }
                break;

            case 2:
                strcpy_s(inbuffer, 13, "Hello Kernel");
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_2, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL);
                if (success)
                {
                    printf("IOCTL_COMMAND_2 sent successfully\n");
                    printf("Message sent to kernel driver: %s\n", inbuffer);
                    printf("Message received from kernel driver: %s\n", outbuffer);
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_2\n");
                }
                break;

            case 3:
                CloseHandle(hDevice);
                return 0;

            default:
                printf("Invalid option\n");
                break;
        }
    }

    CloseHandle(hDevice);

    return 0;
}
