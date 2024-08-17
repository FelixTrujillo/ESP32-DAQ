# Importando modulos
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import tensorflow as tf
from tensorflow.keras import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.losses import MeanSquaredError
from sklearn.metrics import mean_squared_error, mean_absolute_error

# Leyendo datos de hoja de datos
df = pd.read_excel(
    r'C:\\Users\\fe_li\Desktop\\Maestria\\Bicineta-DT\\CodigosPruebas\\TakeDataBicinetaPath-1-2-3.xlsx')
print(df.columns)

# Entradas

aceleracion_k = np.array(df['Aceleracion [V_Acc]']).reshape(-1, 1)
aceleracion_k1 = np.array(np.insert(aceleracion_k[:-1], 0, 0)).reshape(-1, 1)
aceleracion_k2 = np.array(np.insert(aceleracion_k1[:-1], 0, 0)).reshape(-1, 1)

voltaje_k = np.array(df[' Voltaje[V]']).reshape(-1, 1)
voltaje_k1 = np.array(np.insert(voltaje_k[:-1], 0, 0)).reshape(-1, 1)
voltaje_k2 = np.array(np.insert(voltaje_k1[:-1], 0, 0)).reshape(-1, 1)

altitud_k = np.array(df[' Altitud']).reshape(-1, 1)
altitud_k1 = np.array(np.insert(altitud_k[:-1], 0, 0)).reshape(-1, 1)
altitud_k2 = np.array(np.insert(altitud_k1[:-1], 0, 0)).reshape(-1, 1)

# Salidas

velocidad_k = np.array(df['Velocidad [RPM]']).reshape(-1, 1)
velocidad_k1 = np.array(np.insert(velocidad_k[:-1], 0, 0)).reshape(-1, 1)
velocidad_k2 = np.array(np.insert(velocidad_k1[:-1], 0, 0)).reshape(-1, 1)

corriente_k = np.array(df['Corriente[A]']).reshape(-1, 1)
corriente_k1 = np.array(np.insert(corriente_k[:-1], 0, 0)).reshape(-1, 1)
corriente_k2 = np.array(np.insert(corriente_k1[:-1], 0, 0)).reshape(-1, 1)

# print(altitud_k, f"\n\n" ,altitud_k1, f"\n\n", altitud_k2)

X_train = np.array([velocidad_k1, velocidad_k2, corriente_k1, corriente_k2, aceleracion_k, aceleracion_k1,
                   aceleracion_k2, voltaje_k, voltaje_k1, voltaje_k2, altitud_k, altitud_k1, altitud_k2]).transpose().reshape(-1, 13)
y_train = np.array([velocidad_k, corriente_k]).transpose().reshape(-1, 2)
print(X_train.shape)
print(y_train.shape)
print(X_train[:, 0])
# print(y_train)

# Normalizando los datos de entrenamiento
print(f"Aceleracion Max, Min pre normalization: {
      np.max(X_train[:, 0]):0.2f}, {np.min(X_train[:, 0]):0.2f}")
print(f"Voltaje Max, Min pre normalization: {
      np.max(X_train[:, 1]):0.2f}, {np.min(X_train[:, 1]):0.2f}")
print(f"Altitud Max, Min pre normalization: {
      np.max(X_train[:, 2]):0.2f}, {np.min(X_train[:, 2]):0.2f}")
norm_l_x = tf.keras.layers.Normalization(axis=-1)
norm_l_x.adapt(X_train)  # learns mean, variance
Xn = norm_l_x(X_train)
print(f"Aceleracion Max, Min post normalization: {
      np.max(Xn[:, 0]):0.2f}, {np.min(Xn[:, 0]):0.2f}")
print(f"Voltaje     Max, Min post normalization: {
      np.max(Xn[:, 1]):0.2f}, {np.min(Xn[:, 1]):0.2f}")
print(f"Altitud     Max, Min post normalization: {
      np.max(Xn[:, 2]):0.2f}, {np.min(Xn[:, 2]):0.2f}")

print(f"Velocidad Max, Min pre normalization: {
      np.max(y_train[:, 0]):0.2f}, {np.min(y_train[:, 0]):0.2f}")
print(f"Corriente Max, Min pre normalization: {
      np.max(y_train[:, 1]):0.2f}, {np.min(y_train[:, 1]):0.2f}")
norm_l_y = tf.keras.layers.Normalization(axis=-1)
norm_l_y.adapt(y_train)  # learns mean, variance
yn = norm_l_y(y_train)
print(f"Aceleracion Max, Min post normalization: {
      np.max(yn[:, 0]):0.2f}, {np.min(yn[:, 0]):0.2f}")
print(f"Voltaje     Max, Min post normalization: {
      np.max(yn[:, 1]):0.2f}, {np.min(yn[:, 1]):0.2f}")

# obteniendo parametros estadisticos de los datos normalizados
mean_x_train = norm_l_x.mean.numpy()
std_x_train = norm_l_x.variance.numpy()**0.5
mean_y_train = norm_l_y.mean.numpy()
std_y_train = norm_l_y.variance.numpy()**0.5
print(mean_x_train)
print(std_x_train)
print(mean_y_train)
print(std_y_train)

fig, axs = plt.subplots(2, 3, figsize=(14, 10))

ax1 = axs[0, 0]
ax1.scatter(Xn[:, 4], yn[:, 0], marker='x', c='b', label="Data Points")
ax1.legend(fontsize='medium')
ax1.set_ylabel('Velocidad', fontsize='medium')
ax1.set_xlabel('Aceleracion', fontsize='medium')

ax2 = axs[0, 1]
ax2.scatter(Xn[:, 7], yn[:, 0], marker='x', c='b', label="Data Points")
ax2.legend(fontsize='medium')
ax2.set_ylabel('Velocidad', fontsize='medium')
ax2.set_xlabel('Voltaje', fontsize='medium')

ax3 = axs[0, 2]
ax3.scatter(Xn[:, 10], yn[:, 0], marker='x', c='b', label="Data Points")
ax3.legend(fontsize='medium')
ax3.set_ylabel('Velocidad', fontsize='medium')
ax3.set_xlabel('Altitud', fontsize='medium')

ax4 = axs[1, 0]
ax4.scatter(Xn[:, 4], yn[:, 1], marker='x', c='r', label="Data Points")
ax4.legend(fontsize='medium')
ax4.set_ylabel('Corriente', fontsize='medium')
ax4.set_xlabel('Aceleracion', fontsize='medium')

ax5 = axs[1, 1]
ax5.scatter(Xn[:, 7], yn[:, 1], marker='x', c='r', label="Data Points")
ax5.legend(fontsize='medium')
ax5.set_ylabel('Corriente', fontsize='medium')
ax5.set_xlabel('Voltaje', fontsize='medium')

ax6 = axs[1, 2]
ax6.scatter(Xn[:, 10], yn[:, 1], marker='x', c='r', label="Data Points")
ax6.legend(fontsize='medium')
ax6.set_ylabel('Corriente', fontsize='medium')
ax6.set_xlabel('Altitud', fontsize='medium')
plt.show()


model = Sequential([
    Dense(units=4, activation='relu'),
    # Dense(units=3, activation='relu'),
    Dense(units=2, activation='linear')
])

# model.compile(loss=MeanSquaredError())
model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=1e-3),
              loss=MeanSquaredError())
history = model.fit(Xn, yn, epochs=100)
model.summary()
print(f'Parametros: {model.get_weights()}')



# Graficar la curva de pérdidas
plt.figure(figsize=(10, 6))
plt.plot(history.history['loss'], label='Pérdida de entrenamiento')
plt.xlabel('Época', fontsize=12)
plt.ylabel('Pérdida', fontsize=12)
plt.title('Curva de pérdidas del modelo durante el entrenamiento', fontsize=14)
plt.legend()
plt.grid(True)
plt.show()

# Importando datos de testeo
df1 = pd.read_excel(
    r'C:\\Users\\fe_li\Desktop\\Maestria\Bicineta-DT\\CodigosPruebas\\TakeDataBicinetaPath4.xlsx')
print(df.columns)

# Entradas

aceleracion1_k = np.array(df1['Aceleracion [V_Acc]']).reshape(-1, 1)
aceleracion1_k1 = np.array(np.insert(aceleracion1_k[:-1], 0, 0)).reshape(-1, 1)
aceleracion1_k2 = np.array(
    np.insert(aceleracion1_k1[:-1], 0, 0)).reshape(-1, 1)

voltaje1_k = np.array(df1[' Voltaje[V]']).reshape(-1, 1)
voltaje1_k1 = np.array(np.insert(voltaje1_k[:-1], 0, 0)).reshape(-1, 1)
voltaje1_k2 = np.array(np.insert(voltaje1_k1[:-1], 0, 0)).reshape(-1, 1)

altitud1_k = np.array(df1[' Altitud']).reshape(-1, 1)
altitud1_k1 = np.array(np.insert(altitud1_k[:-1], 0, 0)).reshape(-1, 1)
altitud1_k2 = np.array(np.insert(altitud1_k1[:-1], 0, 0)).reshape(-1, 1)

# Salidas

velocidad1_k = np.array(df1['Velocidad [RPM]']).reshape(-1, 1)
velocidad1_k1 = np.array(np.insert(velocidad1_k[:-1], 0, 0)).reshape(-1, 1)
velocidad1_k2 = np.array(np.insert(velocidad1_k1[:-1], 0, 0)).reshape(-1, 1)

corriente1_k = np.array(df1['Corriente[A]']).reshape(-1, 1)
corriente1_k1 = np.array(np.insert(corriente1_k[:-1], 0, 0)).reshape(-1, 1)
corriente1_k2 = np.array(np.insert(corriente1_k1[:-1], 0, 0)).reshape(-1, 1)

# Cargando datos de testeo
X_test = np.array([velocidad1_k1, velocidad1_k2, corriente1_k1, corriente1_k2, aceleracion1_k, aceleracion1_k1,
                  aceleracion1_k2, voltaje1_k, voltaje1_k1, voltaje1_k2, altitud1_k, altitud1_k1, altitud1_k2]).transpose().reshape(-1, 13)
y_test = np.array([velocidad1_k, corriente1_k]).transpose().reshape(-1, 2)
print(X_train.shape)
print(y_train.shape)

print(f"Aceleracion Max, Min pre normalization: {
      np.max(X_test[:, 0]):0.2f}, {np.min(X_test[:, 0]):0.2f}")
print(f"Voltaje Max, Min pre normalization: {
      np.max(X_test[:, 1]):0.2f}, {np.min(X_test[:, 1]):0.2f}")
print(f"Altitud Max, Min pre normalization: {
      np.max(X_test[:, 2]):0.2f}, {np.min(X_test[:, 2]):0.2f}")
norm_l_x_test = tf.keras.layers.Normalization(axis=-1)
norm_l_x_test.adapt(X_test)  # learns mean, variance
Xn_test = norm_l_x_test(X_test)
print(f"Aceleracion Max, Min post normalization: {
      np.max(Xn_test[:, 0]):0.2f}, {np.min(Xn_test[:, 0]):0.2f}")
print(f"Voltaje     Max, Min post normalization: {
      np.max(Xn_test[:, 1]):0.2f}, {np.min(Xn_test[:, 1]):0.2f}")
print(f"Altitud     Max, Min post normalization: {
      np.max(Xn_test[:, 2]):0.2f}, {np.min(Xn_test[:, 2]):0.2f}")

print(f"Velocidad Max, Min pre normalization: {
      np.max(y_test[:, 0]):0.2f}, {np.min(y_test[:, 0]):0.2f}")
print(f"Corriente Max, Min pre normalization: {
      np.max(y_test[:, 1]):0.2f}, {np.min(y_test[:, 1]):0.2f}")
norm_l_y_test = tf.keras.layers.Normalization(axis=-1)
norm_l_y_test.adapt(y_test)  # learns mean, variance
yn_test = norm_l_y_test(y_test)
print(f"Aceleracion Max, Min post normalization: {
      np.max(yn_test[:, 0]):0.2f}, {np.min(yn_test[:, 0]):0.2f}")
print(f"Voltaje     Max, Min post normalization: {
      np.max(yn_test[:, 1]):0.2f}, {np.min(yn_test[:, 1]):0.2f}")


yn_pred = model.predict(Xn_test)


# Evaluar el rendimiento del modelo
mse = mean_squared_error(yn_test, yn_pred)
mae = mean_absolute_error(yn_test, yn_pred)

print(f'Error cuadrático medio (MSE): {mse}')
print(f'Error absoluto medio (MAE): {mae}')

# Graficar las predicciones vs. los valores reales para visualizar el rendimiento del modelo
plt.figure(figsize=(10, 6))
plt.plot(yn_test[:, 0], label='Valores reales - Salida 1', alpha=0.7)
plt.plot(yn_pred[:, 0], label='Predicciones - Salida 1', alpha=0.7)
plt.xlabel('Índice', fontsize=12)
plt.ylabel('Valores', fontsize=12)
plt.title('Comparación de predicciones vs. valores reales (Velocidad normalizada)', fontsize=14)
plt.legend()
plt.grid(True)

plt.figure(figsize=(10, 6))
plt.plot(yn_test[:, 1], label='Valores reales - Salida 2', alpha=0.7)
plt.plot(yn_pred[:, 1], label='Predicciones - Salida 2', alpha=0.7)
plt.xlabel('Índice', fontsize=12)
plt.ylabel('Valores', fontsize=12)
plt.title('Comparación de predicciones vs. valores reales (Corriente normalizada)', fontsize=14)
plt.legend()
plt.grid(True)
plt.show()


# Funcion de desnormalizacion

def denormalize(normalized_data, mean, std_dev):
    return normalized_data * std_dev + mean


# Desnormalizando datos

y_pred = denormalize(yn_pred, mean_y_train, std_y_train)


# Graficando datos desnormalizados

plt.figure(figsize=(10, 6))
plt.plot(y_test[:, 0], label='Valores reales - Salida 1', alpha=0.7)
plt.plot(y_pred[:, 0], label='Predicciones - Salida 1', alpha=0.7)
plt.xlabel('Índice', fontsize=12)
plt.ylabel('Valores', fontsize=12)
plt.title('Comparación de predicciones vs. valores reales (Velocidad)', fontsize=14)
plt.legend()
plt.grid(True)

plt.figure(figsize=(10, 6))
plt.plot(y_test[:, 1], label='Valores reales - Salida 2', alpha=0.7)
plt.plot(y_pred[:, 1], label='Predicciones - Salida 2', alpha=0.7)
plt.xlabel('Índice', fontsize=12)
plt.ylabel('Valores', fontsize=12)
plt.title('Comparación de predicciones vs. valores reales (Corriente)', fontsize=14)
plt.legend()
plt.grid(True)

plt.show()

# Evaluar el rendimiento del modelo
mse = mean_squared_error(y_test, y_pred)
mae = mean_absolute_error(y_test, y_pred)

print(f'Error cuadrático medio (MSE): {mse}')
print(f'Error absoluto medio (MAE): {mae}')
