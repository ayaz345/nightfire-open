#!/usr/bin/python

from __future__ import print_function

from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import GRU
from keras.models import load_model
from keras import backend as K

import numpy as np

def printVector(f, vector, name):
    v = np.reshape(vector, (-1));
    #print('static const float ', name, '[', len(v), '] = \n', file=f)
    f.write(f'static const opus_int16 {name}[{len(v)}] = {{\n   ')
    for i in range(0, len(v)):
        f.write(f'{int(round(8192 * v[i]))}')
        if (i!=len(v)-1):
            f.write(',')
        else:
            break;
        if (i%8==7):
            f.write("\n   ")
        else:
            f.write(" ")
    #print(v, file=f)
    f.write('\n};\n\n')
    return;

def binary_crossentrop2(y_true, y_pred):
        return K.mean(2*K.abs(y_true-0.5) * K.binary_crossentropy(y_pred, y_true), axis=-1)


model = load_model("weights.hdf5", custom_objects={'binary_crossentrop2': binary_crossentrop2})

weights = model.get_weights()

with open('rnn_weights.c', 'w') as f:
    f.write('/*This file is automatically generated from a Keras model*/\n\n')
    f.write('#ifdef HAVE_CONFIG_H\n#include "config.h"\n#endif\n\n#include "mlp.h"\n\n')

    printVector(f, weights[0], 'layer0_weights')
    printVector(f, weights[1], 'layer0_bias')
    printVector(f, weights[2], 'layer1_weights')
    printVector(f, weights[3], 'layer1_recur_weights')
    printVector(f, weights[4], 'layer1_bias')
    printVector(f, weights[5], 'layer2_weights')
    printVector(f, weights[6], 'layer2_bias')

    f.write('const DenseLayer layer0 = {\n   layer0_bias,\n   layer0_weights,\n   25, 16, 0\n};\n\n')
    f.write('const GRULayer layer1 = {\n   layer1_bias,\n   layer1_weights,\n   layer1_recur_weights,\n   16, 12\n};\n\n')
    f.write('const DenseLayer layer2 = {\n   layer2_bias,\n   layer2_weights,\n   12, 2, 1\n};\n\n')
