import numpy as np
from keras.layers import Input, Dense, Conv2D, MaxPooling2D, UpSampling2D
from keras.models import Model
import keras
import matplotlib.pyplot as plt

import tensorflow as tf

data_smooth = np.empty(0)
data_noisy = np.empty(0)

temp_smooth = np.empty(0)
temp_noisy = np.empty(0)

input_size = 500

for i in range(input_size):
	print("Loading ", i)
	with open("data/img" + str(i) + ".bin", 'rb') as f:
		data = np.fromfile(f, dtype=np.float32)
		temp_smooth = np.append(temp_smooth, data)

	with open("data/noisy" + str(i) + ".bin", 'rb') as f:
		data = np.fromfile(f, dtype=np.float32)
		data_noisy = np.append(data_noisy, data)

	if i % 50 == 0 and i != 0:
		data_smooth = np.append(data_smooth, temp_smooth)
		temp_smooth = np.empty(0)
		data_noisy = np.append(data_noisy, temp_noisy)
		temp_noisy = np.empty(0)

data_smooth = np.append(data_smooth, temp_smooth)
data_noisy = np.append(data_noisy, temp_noisy)

data_smooth = np.reshape(data_smooth, [input_size , 128, 128, -1])
data_noisy = np.reshape(data_noisy, [input_size , 128, 128, -1])
print(data_smooth.shape)



input = Input(shape=(128, 128, 3))

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

x = UpSampling2D((2, 2))(encoded)
x = keras.layers.Add()([x4, x]);
x = Conv2D(76, (3, 3), activation='relu', padding='same')(x)

x = UpSampling2D((2, 2))(x)
x = keras.layers.Add()([x3, x]);
x = Conv2D(57, (3, 3), activation='relu', padding='same')(x)

x = UpSampling2D((2, 2))(x)
x = keras.layers.Add()([x2, x]);
x = Conv2D(43, (3, 3), activation='relu', padding='same')(x)

x = UpSampling2D((2, 2))(x)
x = keras.layers.Add()([x1, x]);
x = Conv2D(32, (3, 3), activation='relu', padding='same')(x)

x = UpSampling2D((2, 2))(x)
x = keras.layers.Add()([x0, x]);
decoded = Conv2D(3, (3, 3), activation='sigmoid', padding='same')(x)


#x = keras.layers.Add()([x1, y1])


autoencoder = Model(input, decoded)
autoencoder.compile(optimizer='adam', loss='mean_absolute_error')

autoencoder.fit(data_noisy, data_smooth,
                epochs=100,
                batch_size=100,
                shuffle=True,
                validation_data=(data_noisy, data_smooth))

smoothed = autoencoder.predict(data_noisy)

offset = 300

n = 12
plt.figure()
for i in range(n):
	# display original
	ax = plt.subplot(3, n, i+1)
	plt.imshow(data_noisy[i+offset])
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)

	# display reconstruction
	ax = plt.subplot(3, n, i + n+1)
	plt.imshow(smoothed[i+offset])
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)

	ax = plt.subplot(3, n, i + 2*n+1)
	plt.imshow(data_smooth[i+offset])
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)
plt.show()