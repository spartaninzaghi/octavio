# octavIO
octavIO 25-Key Scalable MIDI Keyboard using ESP32-S3 (interoperable with ESP32 WROOM 32 and other boards after prototype 4)

The current version includes prototype with ESP32-S3-DevkitC1-N16R8. The schematic and images of the hardware setup for the current protoype are included below.

# Schematic
![v2 1_hardware_schematic_grid](https://github.com/user-attachments/assets/32545c08-1097-410c-92c7-284a4804e36f)

# Hardware Setup
![v3 1_full-system_plan](https://github.com/user-attachments/assets/9ba8d7f0-1f8c-473b-80a9-3878e8d2335a)

# 3-Resistor Voltage Divider DC Offset Circuit
![image](https://github.com/user-attachments/assets/008523da-6e5c-458b-8677-bd4b666d11bf)

![piezo_3Resistor_voltage_divider_DC_offset_circuit_trace_all_plots](https://github.com/user-attachments/assets/c8a88271-0e7a-4746-850a-9857c29c6a70)

![image](https://github.com/user-attachments/assets/554ee7dc-420a-4545-b603-cf756b1ef964)

# Piezo disk Voltage Scope Reading
![scope_reading_of_piezo_voltage_response_when_tapped_and_released](https://github.com/user-attachments/assets/cbbe23be-2625-4300-80d4-fe3b64b80cf5)

# Piezo Arduino Serial Plot Sample with DC Offset (~1.571V, signal: +/- ~20V)
![ardunio_serial_plot_of_piezo_dc_offset](https://github.com/user-attachments/assets/d99f8d51-c607-4914-b35d-a2260b8538d9)
