# Yocto MPU6050 Layer for BeagleBone Black

![GitHub repo size](https://img.shields.io/github/repo-size/AnandPakhode/bbb-yocto-mpu6050)
![License](https://img.shields.io/github/license/AnandPakhode/bbb-yocto-mpu6050)

This repository contains a **Yocto layer (`meta-mpu6050`)** for integrating the **MPU6050 accelerometer and gyroscope sensor** with the **BeagleBone Black**. 
It provides BitBake recipes and example source files to build the sensor application within a Yocto Project.

---

## 🌟 Features

- Lightweight Yocto layer for MPU6050 integration  
- Example BitBake recipes for building MPU6050 applications  
- Easy-to-add layer for any Yocto-based BBB project  
- MIT licensed for free use and modification  

---

## 📂 Layer Structure

meta-mpu6050/
├── conf/
│   └── layer.conf           # Layer configuration for Yocto (defines layer priority, BBPATH, etc.)
├── recipes-example/
│   ├── example/
│   │   └── example_0.1.bb   # Sample BitBake recipe for demonstration
│   └── mpu6050/
│       ├── mpu6050.bb       # BitBake recipe for building MPU6050 application
│       └── files/
│           └── mpu6050.c    # Source code for MPU6050 sensor interface
├── COPYING.MIT               # MIT license file
└── README.md                 # Documentation for the layer
