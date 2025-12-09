# 🛠️ API Desktop Client – Aplicación de Pruebas de API (Tipo Postman)

Aplicación de escritorio desarrollada con el objetivo de **aprender a consumir APIs** y experimentar con solicitudes HTTP desde un entorno propio.  
La hice tanto para practicar desarrollando aplicaciones de escritorio como para practicar consumo de APIs.

---

## 🎯 Objetivo del proyecto
El proyecto nació como una práctica personal para entender:

- Cómo estructurar solicitudes HTTP (GET, POST, PUT, DELETE)
- Cómo manejar URLs y cuerpo de las peticiones
- Cómo interpretar las respuestas de un servidor

---

## ✨ Características actuales

### ✔️ Gestión de modelos
- Permite crear **modelos de datos**.
- Los modelos se guardan temporalmente en memoria.
- Cada modelo genera automáticamente un endpoint basado en su nombre.

### ✔️ Generación de endpoints
- Los endpoints se clasifican por método: **GET, POST, PUT y DELETE**.

### ✔️ Configuración de API
- Campo para ingresar la **URL base** de la API.
- Construcción dinámica del endpoint final.

### ✔️ Cuerpo de la solicitud
- Permite ingresar cuerpo (body) en métodos **POST** y **PUT**.
- Se genera un JSON en base al modelo armado

### ✔️ Visualización de respuestas
- Panel de salida donde se muestra el **contenido devuelto por el servidor**.

### ✔️ Header básico de autorización
- Campo simple para enviar el header **Authorization**.

---

## 🚀 Implementaciones futuras

### 🔧 Mejoras en modelos
- Persistencia local de los modelos creados.
- Administración avanzada de modelos (editar, duplicar, eliminar).

### 🔧 Manejo avanzado de respuestas
- Permitir definir **modelos de respuesta**.
- Actualmente las respuestas se leen esperando un campo "data" o "message" en caso de error.

### 🔧 Mejor representación del resultado
- Mostrar **código de estado HTTP**.
- Mostrar tiempo de respuesta.

### 🔧 Headers avanzados
- Panel para agregar headers comunes o personalizados.

### 🔧 URL Parameters
- Permitir incluir en las solicitudes parámetros por URL

### 🔧 Refactorización de código
- Mejorar el código que actualmente está la mayor parte centralizada en el "mainwindow" y muy poco entendible.
