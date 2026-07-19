# Magic Button — firmware IDF

Este repo faz parte do projeto de um interruptor inteligente que envia o estado de um interruptor simples para seu Hub de automação (Alexa, Google Home, Apple Casa, Home Assistant) via protocolo Matter. 

Contem o Firmware Matter (**On/Off Plugin Unit**) para ESP32-C6 Super Mini criado com a stack da Espressif.

Build, Flash e testes na placa: ver [../README.md](../README.md).

## Fontes

| Arquivo | Papel |
|---------|--------|
| `app_main.cpp` | Nó Matter, endpoint Plugin Unit, callbacks |
| `app_driver.cpp` | GPIO14 FSM, GPIO8 decommission, GPIO15 LED |
| `app_priv.h` | Pinos e API do driver |
| `CHIPProjectConfig.h` | Vendor/product names e VID/PID |
