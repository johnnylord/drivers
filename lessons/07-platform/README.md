# Platform devices & drivers

## Start of Kernel with Device Tree Parsing
The program entry for kernel is `start_kernel` function in `init/main.c` file
```c
asmlinkage __visible void __init start_kernel(void)
{
	char *command_line;
	char *after_dashes;
    // ...
    setup_arch(&command_line); // Architecture dependent codes
    // ...
}
```
Assume we are developing with an arm board, the setup code is in `arch/arm64/kernel/setup.c`
```c
void __init setup_arch(char **cmdline_p)
{
	pr_info("Boot CPU: AArch64 Processor [%08x]\n", read_cpuid_id());

    // ...

	*cmdline_p = boot_command_line;     // This stores the boot cmdline from the bootloader

    // ...

	setup_machine_fdt(__fdt_pointer);   // Perform scanning on the device tree (Parse essential nodes only)

	parse_early_param();

    // ...

	if (acpi_disabled)
		unflatten_device_tree();

    // ...
}

```
After entering into `setup_machine_fdt`, it will remap the address of device tree and perform early scanning on the device tree.
```c
static void __init setup_machine_fdt(phys_addr_t dt_phys)
{
	void *dt_virt = fixmap_remap_fdt(dt_phys);

	if (!dt_virt || !early_init_dt_scan(dt_virt)) {
		pr_crit("\n"
			"Error: invalid device tree blob at physical address %pa (virtual address 0x%p)\n"
			"The dtb must be 8-byte aligned and must not exceed 2 MB in size\n"
			"\nPlease check your bootloader.",
			&dt_phys, dt_virt);
        // ...
	}
    // ...
}
```
Before performing the device tree scanning, it will first verify if the device tree blob is valid with correct magic header of device tree.
```c
bool __init early_init_dt_scan(void *params)
{
	bool status;

	status = early_init_dt_verify(params); // Check the device tree header
	if (!status)
		return false;

	early_init_dt_scan_nodes(); // Perform early scanning
	return true;
}
```
In early scanning, kernel will process main three nodes, `chosen/`, `/`, and `memory`.
```c
void __init early_init_dt_scan_nodes(void)
{
	/* Retrieve various information from the /chosen node */
	of_scan_flat_dt(early_init_dt_scan_chosen, boot_command_line);

	/* Initialize {size,address}-cells info */
	of_scan_flat_dt(early_init_dt_scan_root, NULL);

	/* Setup memory, calling early_init_dt_add_memory_arch */
	of_scan_flat_dt(early_init_dt_scan_memory, NULL);
}
```


## How platform device get populated

## How platform device get binded with platform driver


## NOTES:
- Platform devices are not discoverable.
- Platform devices are usualy direclty addressed from a CPU bus
- Platform devices are given a `name` for **driver binding**
- `platform_device` is a higher-level data structure from `device`
- **discovery/enumeration** is handled outside the drivers
- drivers provide `probe` and `remove` methods
` `probe` ingeneral veritgy the existence of the device hardware
- if the deivce is not hot-pluggalbe, the logits of code are commonly place in the modle init function
- To bind the platform device to a driver, the device must be registered with the same name which driver is registered
