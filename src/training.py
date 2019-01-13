import numpy as np
from keras.layers import Input, Dense, Conv2D, MaxPooling2D, UpSampling2D
from keras.models import Model
from keras.models import load_model
import sys
import keras
import matplotlib.pyplot as plt
import random
import time
import tensorflow as tf

from keras.backend.tensorflow_backend import set_session
config = tf.ConfigProto()
config.gpu_options.per_process_gpu_memory_fraction = 0.7
set_session(tf.Session(config=config))


input_size = 36
load = True
training = False
filename = "model.h5"


def new_autoencoder():
	input = Input(shape=(128, 128, 6))
	x0 = Conv2D(32, (3, 3), activation='relu', padding='same')(input)
	x = MaxPooling2D((2, 2), padding='same')(x0)
	x1 = Conv2D(43, (3, 3), activation='relu', padding='same')(x)
	x = MaxPooling2D((2, 2), padding='same')(x1)
	x2 = Conv2D(57, (3, 3), activation='relu', padding='same')(x)
	x = MaxPooling2D((2, 2), padding='same')(x2)
	x3 = Conv2D(76, (3, 3), activation='relu', padding='same')(x)
	x = MaxPooling2D((2, 2), padding='same')(x3)
	x4 = Conv2D(101, (3, 3), activation='relu', padding='same')(x)
	x = MaxPooling2D((2, 2), padding='same')(x4)
	encoded = Conv2D(101, (3, 3), activation='relu', padding='same')(x)
	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x4, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(76, (3, 3), activation='relu', padding='same')(x)
	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x3, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(57, (3, 3), activation='relu', padding='same')(x)
	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x2, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(43, (3, 3), activation='relu', padding='same')(x)
	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x1, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(32, (3, 3), activation='relu', padding='same')(x)
	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x0, x]);
	x = keras.layers.ReLU()(x)
	decoded = Conv2D(3, (3, 3), activation='sigmoid', padding='same')(x)
	autoencoder = Model(input, decoded)
	autoencoder.compile(optimizer='Adam', loss='mean_absolute_error')
	return autoencoder


if load:
	print("Loading saved model: '", filename, "'")
	autoencoder = load_model("model.h5")
else:
	print("Creating new autoencoder")
	autoencoder = new_autoencoder()


data_smooth = np.empty(0)
data_noisy = np.empty(0)
temp_smooth = np.empty(0)
temp_noisy = np.empty(0)
offset = 36*10
for i in range(input_size):
	percent = round(100 * i / input_size)
	sys.stdout.write("\rLoading training data: " + str(percent) + "%")
	sys.stdout.flush()
	with open("data/img" + str(i+offset) + ".bin", 'rb') as f:
		data = np.fromfile(f, dtype=np.float32)
		temp_smooth = np.append(temp_smooth, data)
	with open("data/noisy" + str(i+offset) + ".bin", 'rb') as f:
		data = np.fromfile(f, dtype=np.float32)
		temp_noisy = np.append(temp_noisy, data)
	if i % 50 == 0 and i != 0:
		data_smooth = np.append(data_smooth, temp_smooth)
		temp_smooth = np.empty(0)
		data_noisy = np.append(data_noisy, temp_noisy)
		temp_noisy = np.empty(0)
data_smooth = np.append(data_smooth, temp_smooth)
data_noisy = np.append(data_noisy, temp_noisy)
data_smooth = np.reshape(data_smooth, [input_size , 128, 128, -1])
data_noisy = np.reshape(data_noisy, [input_size , 128, 128, -1])
print()

if training:
	train_index = round(input_size * 9.0 / 10.0)
	training_input = data_noisy[:train_index]
	training_output = data_smooth[:train_index]
	validation_input = data_noisy[train_index:input_size]
	validation_output = data_smooth[train_index:input_size]
	while True:
		autoencoder.fit(training_input, training_output, 
						epochs=20, 
						batch_size=200, 
						shuffle=True, 
						validation_data=(validation_input, validation_output))
		autoencoder.save("model.h5")
else:
	smoothed = autoencoder.predict(data_noisy)

	n = 6
	plt.figure(frameon=False)
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(smoothed[i + j * n])
			ax.set_axis_off()
			ax.axis('off')

	plt.figure(frameon=False)
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(data_noisy[i + j * n, :,:,:3])
			ax.get_xaxis().set_visible(False)
			ax.get_yaxis().set_visible(False)
			ax.axis('off')
	"""
	plt.figure(frameon=False)
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(data_noisy[i + j * n, :,:,3:6])
			ax.get_xaxis().set_visible(False)
			ax.get_yaxis().set_visible(False)
			ax.axis('off')

	plt.figure(frameon=False)
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(data_noisy[i + j * n, :,:,5], cmap='Greys')
			ax.get_xaxis().set_visible(False)
			ax.get_yaxis().set_visible(False)
			ax.axis('off')
	"""
	plt.subplots_adjust(wspace=0, hspace=0)
	plt.show()