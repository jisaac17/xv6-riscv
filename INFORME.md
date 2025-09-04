## Documentación de pasos de instalación:

después de descargar ubuntu y configurar git, iniciamos con los pasos del documento

## 1. Clonar repositorio:

Realizo un fork desde el repositorio de xv6 a desde mi cuenta de GitHub.

después clono mi propio repositorio desde ubuntu con la siguiente línea git clone https://github.com/jisaac17/xv6-riscv.

## 2. Crear una Nueva Rama

Se crea con el siguiente comando git checkout -b jisaac17_t0 y de forma paralela se ubica en esa rama.

## 3. Instalar Dependencias

primero es necesario corre esta línea 
 sudo apt-get update

despues se instalan dependencias utilizando la siguiente linea que descarga las dependencias principales para utilizar xv6 con
sudo apt-get install -y build-essential qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

## 4. Compilar xv6
se compila utilizando make

## 5. Ejecutar xv6
se ejecuta (También se compila si todavía no se ha compilado) utilizando 
make qemu

## 6. Verificar la Instalación
sé probo los siguientes comandos
$ ls
$ echo "Hola xv6"
$ cat README
revisar pantallazo adjunto


## problemas que encontrados al instalar xv6

Al instalar las dependencias no se descargan todos los paquetes de manera correcta esto ocurre porque el índice de paquetes está desactualizado o el mirror cambió la versión.

la solución es refrescar el índice con las siguientes líneas

sudo rm -rf /var/lib/apt/lists/*
sudo apt-get update

y después volver a instalar las dependencias con 

sudo apt-get install gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu


Otro problema al correr xv6, es que no se compila con make qemu, porque xv6 compila varias herramientas de usuario para el host con el gcc nativo de mi Ubuntu/WSL, no con el cross-compiler RISC-V.

Se soluciona instalando el toolchain de desarrollo del host con las siguientes líneas

sudo apt-get update
sudo apt-get install -y build-essential   # gcc, g++, make, etc.
