Compilar: gcc Test.c -L .

Tanto como FileLibrary.c como Test.c deben estar en el mismo directorio.

En caso de que se quiera usar FileLibrary.c como librería en otro directorio, se deberá incluir el encabezado #include "FileLibrary.c" y poner como parámetro el directorio en donde FileLibrary.c se encuentra al momento de compilar: gcc Archivo.c -L /directorio/del/archivo/bla/bla

Use el archivo Test.c para probar los métodos de la librería.