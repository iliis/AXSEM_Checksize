AXSEM_Checksize
===============

A short tool to verify if a Intel HEX firmware image is compatible with the AXSEM bootloader.
Its main purpose is the integration into an existing project to prevent the overwriting of
sensitive data such as the factory calibration or a bootloader.


Usage
-----

    ./checksize [-p|--protect ADDRESS] [-f|--file IMAGE.hex]

The IMAGE.hex has to be in Intel HEX format. If no file is provided, input is read from
standard input.

ADDRESS is a hexadecimal address
(without '0x') which marks the start of the sensitive area.

For example, add this to the Post-build steps in Code::Blocks to prevent axsdb from
overwriting the factory calibration data.

	$(PROJECT_DIR)checksize --file $(PROJECT_DIR)$(TARGET_OUTPUT_FILE) --protect FC00
