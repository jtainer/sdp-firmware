#!/bin/bash
rm src/*
rm inc/*
rm lib/usb/*
cp ~/STM32CubeIDE/workspace_1.13.2/sdp_mainboard/Core/Src/* src/
cp ~/STM32CubeIDE/workspace_1.13.2/sdp_mainboard/Core/Inc/* inc/
cp ~/STM32CubeIDE/workspace_1.13.2/sdp_mainboard/USB_DEVICE/App/* lib/usb/
cp ~/STM32CubeIDE/workspace_1.13.2/sdp_mainboard/USB_DEVICE/Target/* lib/usb/


