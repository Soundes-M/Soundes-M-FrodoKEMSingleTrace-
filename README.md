# Soundes-M-FrodoKEMSingleTrace-
This code contains two parts: the simulation of a single trace attack against FrodoKEM, and a power analysis attack agains the Gaussian sampler callsed in FrodoKEM, and countermeasure implementation of the Gaussian sample. First, excute:

```
git clone https://github.com/Soundes-M/Soundes-M-FrodoKEMSingleTrace-.git
```

run:
```
make
```

then run:
```
./frodo/test_KEM
```

If the simulation is correct, you should see the message ```Test PASSED```

To run the power analysis, run:
```
cd chipwhisperer/
```

Then:
```
jupyter notebook
```

then go to the folder jupyter and open the file Attack on FrodoKEM (PA).ipynb
and excute it.
The code is commented, 
If wou use an STM32F4 victim board, you will be able to build a classifier -which- based on the power consumption trace can detect the Gaussian samples.

In case you are using an STM32F3 board, the samples can be detected with naked eye from the traces.
