#!/bin/bash

insmod ./kernel_module_build/cpu_time_measure.ko
sleep 15s
rmmod cpu_time_measure
journalctl --since "5 minutes ago" | grep cpu_time_measure