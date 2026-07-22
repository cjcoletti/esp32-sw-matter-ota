# Enclosure — Magic Button (ESP32-C6 Super Mini + mini fonte)

[![Português](https://img.shields.io/badge/Language-Português-blue)](README.md)[![English](https://img.shields.io/badge/_English-inactive)](README.en.md)

Caixa compacta para o módulo **Magic Button** (ESP32-C6 Super Mini + mini fonte Hilink).

![Caixa 3D impressa — base e tampa](case-matter-switch.png)

> Modelo paramétrico **V16.3** — placas dispostas lado a lado, trilhos de encaixe por pressão, furo Ø8 centralizado no vão e furo para a USB-C para deploy/debug da ESP32 via JTAG.

## Arquivos

| Arquivo | Uso |
|---------|-----|
| `base.stl` | Base da caixa (imprimir) |
| `tampa.stl` | Tampa (imprimir) |
| `case-matter-switch.png` | Foto da caixa impressa (base + tampa) |

## Dimensões externas

| Eixo | mm |
|------|-----|
| Largura (X, fonte \|\| ESP) | **45,0** |
| Profundidade (Y, eixos longos das placas) | **38,3** |
| Altura montada | **24,5** (base 22,5 + tampa 2,0) |

Paredes **2 mm**; cantos externos arredondados (**r = 3 mm**).

## Componentes

| Módulo | Dimensões (mm) | Orientação |
|--------|----------------|------------|
| Fonte | 20 × 34 × (15 + 5) (largura × profundidade × altura (fonte + pinos)) | Deitada |
| ESP32-C6 | 18 × 25 × 20 (largura × profundidade × altura) | Em pé |

Folga de encaixe no CAD: **0,3 mm** (`folga_encaixe`).

## Layout interno

Vista superior (tampa removida):

```
  Y=38 (traseira)
  +--------+----+--------+
  |        |boss|        |
  |  FONTE | 2mm|  ESP   |
  | trilho |vão | trilho |
  |  18 mm |    |  21 mm |
  |        |boss|        |
  +--------+-Ø8-+--USB-C-+
  Y=0 (frente)
```

- **Fonte (esquerda):** trilhos laterais com vão interno de **15 mm**; guia por pressão até o fundo (altura do guia **4 mm**).
- **ESP (direita):** trilhos com vão interno de **18 mm**; profundidade total da placa (**25,3 mm** com folga).
- **Vão central:** **2 mm** entre os trilhos (pinos / headers).
- **Trilhos:** abas de **1,5 mm** de cada lado; a placa desliza entre elas até assentar no fundo.

## Aberturas na face frontal (Y = 0)

| Abertura | Especificação |
|----------|---------------|
| USB-C | Perfil stadium **10,0 × 4,5 mm** (horizontal), alinhado à ESP |
| Fios | **Ø 8 mm**, centralizado no vão (X = centro do vão), logo abaixo do boss frontal |

## Fixação

Tampa com 2 furos de parafusos de **2× M2 × 6 mm**, centralizados na largura (eixo do vão):

- Bosses na parede **frontal** e **traseira** (altura **7,5 mm**).
- Piloto cego **Ø 2,0 mm**, profundidade **6 mm** na caixa.
- Furo passante **Ø 2,4 mm** na tampa, com rebaixo de cabeça (**Ø 4,2 mm**).

## Impressão sugerida

- PLA ou PETG, camada **0,2 mm**, **2–3 perímetros**, 15–20% infill
- Sem suporte: base com fundo no leito; tampa plana no leito
- Testar encaixe dos trilhos e do USB-C antes de fechar a caixa
