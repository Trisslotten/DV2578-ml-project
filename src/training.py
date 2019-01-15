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


input_size = 36*200
offset = 36
load = True
training = True
filename = "model.h5"


def new_autoencoder():
	size1 = 32
	size2 = 43
	size3 = 57
	size4 = 76
	size5 = 101
	input = Input(shape=(128, 128, 6))

	x = Conv2D(size1, (3, 3) , padding='same')(input)
	x0 = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = MaxPooling2D((2, 2), padding='same')(x0)
	x = Conv2D(size2, (3, 3), padding='same')(x)
	x1 = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = MaxPooling2D((2, 2), padding='same')(x1)
	x = Conv2D(size3, (3, 3), padding='same')(x)
	x2 = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = MaxPooling2D((2, 2), padding='same')(x2)
	x = Conv2D(size4, (3, 3), padding='same')(x)
	x3 = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = MaxPooling2D((2, 2), padding='same')(x3)
	x = Conv2D(size5, (3, 3), padding='same')(x)
	x4 = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = MaxPooling2D((2, 2), padding='same')(x4)
	x = Conv2D(size5, (3, 3), padding='same')(x)
	encoded = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = UpSampling2D((2, 2))(encoded)
	x = keras.layers.Add()([x4, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(size4, (3, 3), padding='same')(x)
	x = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x3, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(size3, (3, 3), padding='same')(x)
	x = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x2, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(size2, (3, 3), padding='same')(x)
	x = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x1, x]);
	x = keras.layers.ReLU()(x)
	x = Conv2D(size1, (3, 3), padding='same')(x)
	x = keras.layers.LeakyReLU(alpha=0.1)(x)

	x = UpSampling2D((2, 2))(x)
	x = keras.layers.Add()([x0, x]);
	x = keras.layers.ReLU()(x)
	decoded = Conv2D(3, (3, 3), activation='linear', padding='same')(x)
	autoencoder = Model(input, decoded)
	autoencoder.compile(optimizer=keras.optimizers.Adam(lr=0.001, beta_1=0.9, beta_2=0.99), loss='mean_absolute_error')
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
	if i % 30 == 0 and i != 0:
		data_smooth = np.append(data_smooth, temp_smooth)
		temp_smooth = np.empty(0)
		data_noisy = np.append(data_noisy, temp_noisy)
		temp_noisy = np.empty(0)
data_smooth = np.append(data_smooth, temp_smooth)
data_noisy = np.append(data_noisy, temp_noisy)
data_smooth = np.reshape(data_smooth, [input_size , 128, 128, -1])
data_noisy = np.reshape(data_noisy, [input_size , 128, 128, -1])

#data_smooth = np.power(data_smooth, 0.2)
#data_noisy[:,:,:,:3] = np.power(data_noisy[:,:,:,:3], 0.2)

print()

if training:
	train_index = round(input_size * 9.0 / 10.0)
	training_input = data_noisy[:train_index]
	training_output = data_smooth[:train_index]
	validation_input = data_noisy[train_index:input_size]
	validation_output = data_smooth[train_index:input_size]
	while True:
		autoencoder.fit(training_input, training_output, 
						epochs=50, 
						batch_size=100, 
						shuffle=True, 
						validation_data=(validation_input, validation_output))
		autoencoder.save("model.h5")
else:
	smoothed = autoencoder.predict(data_noisy)
	
	#smoothed = np.power(smoothed, 1/0.2)
	#data_smooth = np.power(data_smooth, 1/0.2)
	#data_noisy[:,:,:,:3] = np.power(data_noisy[:,:,:,:3], 1/0.2)

	n = 6
	plt.figure("Output - Predicted")
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(smoothed[i + j * n])
			ax.set_axis_off()
			ax.axis('off')


	plt.figure("Input - Noisy")
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(data_noisy[i + j * n, :,:,:3])
			ax.get_xaxis().set_visible(False)
			ax.get_yaxis().set_visible(False)
			ax.axis('off')

	plt.figure("Wanted - high sample per pixel")
	plt.subplots_adjust(wspace=0, hspace=0)
	for i in range(n):
		for j in range(n):
			ax = plt.subplot(n, n, i + j * n + 1)
			plt.imshow(data_smooth[i + j * n])
			ax.set_axis_off()
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