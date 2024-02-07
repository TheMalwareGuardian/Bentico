# Laboratorio


## Título

Desarrollo de Rootkits de modo Kernel en Windows


## Descripción

Un rootkit es una herramienta esencial dentro del arsenal que todo experto en ciberseguridad debe poseer, y también un vector de ataque utilizado por actores maliciosos. Al adentrarnos en el fascinante mundo de su desarrollo, transformaremos ese concepto a un formato comprensible y aplicable, obteniendo un malware con capacidades ilimitadas de persistencia y control sobre una máquina. Durante esta formación, todos los asistentes llevarán a cabo la implementación de un Rootkit de modo kernel, orientado a sistemas operativos Windows 10 y 11, desde cero.

Primero, a través de casos prácticos, obtendremos un conocimiento profundo sobre conceptos relacionados con el kernel de windows, como las estructuras opacas, los drivers, los mecanismos de protección existentes, y un largo etcétera de fundamentos asociados a esta temática.

Una vez explorados los aspectos clave de la seguridad del kernel y cómo los rootkits pueden manipular este nivel para lograr el control del sistema, crearemos un entorno de desarrollo y construiremos la base, un driver de modo kernel que modifique estructuras y cuente con funcionalidades que permitan la ejecución de código.

En el tramo final, una vez completada la implementación, se proporcionará una recopilación de materiales y guías adecuadas para que los asistentes profundicen en dicha área de conocimiento.


## Objetivos

Esta formación es el punto de partida hacia el fascinante mundo de los Rootkits y su desarrollo en sistemas operativos Windows. A lo largo de la sesión, los asistentes adquirirán todos los conocimientos necesarios para abordar un proyecto orientado a la creación de este tipo de malware, aplicando dicha experiencia en la resolución de un ejercicio práctico de ciberseguridad ofensiva.

Los alumnos explorarán el kernel de Windows, su depuración, las estructuras no documentadas, los mecanismos de protección existentes, la creación de un entorno de laboratorio y el desarrollo de un rootkit para windows 10 y 11, entre otros aspectos cruciales de este campo, desde cero.

Al transformar conceptos complejos en un formato comprensible y aplicable, la finalidad es proporcionar, a aquellos interesados en la materia, las herramientas necesarias para llevar a cabo el desarrollo de un software con tal capacidad de control y persistencia en un sistema Windows.


## A quién va dirigido

Los fundamentos teóricos y prácticos que se impartirán, están especialmente diseñados para aquellos con la inquietud de iniciarse en el desarrollo de Rootkits en Windows:

- Profesionales dedicados a la ciberseguridad ofensiva
- Cuerpos y Fuerzas de Seguridad
- Desarrolladores
- Estudiantes y docentes
- Cualquier persona interesada en aprender este área de conocimiento


## Requisitos: Conocimientos

Este área de especialización es extremadamente amplia pero no se requiere tener un conocimiento previo de cada uno de los temas a tratar en la formación. Simplemente es importante, para el correcto seguimiento, disponer de conocimientos básicos para una persona entusiasta de la ciberseguridad:

- Bases en ciberseguridad ofensiva.
- Manejo de sistemas operativos Windows.
- Programación en C y C++.
- Muchas ganas de aprender.


## Requisitos: Técnicos

Los alumnos deben disponer de un equipo con acceso de administrador, que cuente con características aceptables en cuanto a CPU y memoria RAM para crear un laboratorio local de desarrollo. Este laboratorio consistirá en, previa instalación de un software de virtualización como VMWare Workstation o VirtualBox, dos máquinas virtuales con Windows 10 y 11 Home base, que cada alumno debe crear previamente al día de la formación.

Para evitar un rendimiento bajo y esperas innecesarias sobre una de las máquinas, en la que se realizará el desarrollo, esta debe disponer de mínimo 8 GB de RAM y tener instalado Visual Studio 2022 Community con la opción 'Desarrollo para el escritorio con C++'.


## Contenido

Un secreto es simplemente conocimiento que se mantiene parcialmente desconocido, por lo que para comenzar la formación se hará una exposición de conceptos relacionados con Rootkits, transformándolos en conocimiento claro y accesible para el conjunto de los asistentes. Una vez explorados estos aspectos clave, se llevará a cabo el despliegue del entorno, desarrollando una prueba de concepto que servirá como base.

Dando seguimiento a la parte práctica, se comenzarán a desarrollar y ejecutar pruebas sobre las máquinas del laboratorio, con el objetivo de construir el Rootkit, lo que implica conocer el kernel de Windows, las estructuras opacas, los mecanismos de protección, las funcionalidades a implementar en un driver de modo kernel, entre muchos otros puntos que exploraremos de forma práctica y guiada.

Finalmente, completada la implementación, se proporcionarán recursos para profundizar en todos aquellos puntos que hayan sido de interés, además de compartir ideas sobre el área de conocimiento.


## Agenda (I) - Introducción

- Exposición de ejercicio práctico
- Presentación de objetivos
- Iniciación al mundo de los Rootkits
- Desmitificación de conceptos clave
    - Kernel
    - Estructuras
    - Drivers
    - Patchguard


## Agenda (II) - Laboratorio

- Instalación del entorno
- Proceso de infección
- Prueba de concepto
    - KMDF
    - DSE
    - Dbgview
- Rootkits in the wild


## Agenda (III) - Rootkits

- Depuración local y remota
    - WinDbg
    - Operaciones
    - Estructuras opacas
- Drivers avanzados
    - Modelos
    - Funcionalidades
        - Ocultar conexiones
        - Proteger procesos
        - Modificar componentes
        - Establecer persistencia
    - Comunicación
        - IOCTLs
        - IRPs
    - Aplicación del ejercicio
- Materiales, reconocimiento y conclusiones


## Recursos

* ***[Awesome Bootkits & Rootkits Development](https://github.com/TheMalwareGuardian/Awesome-Bootkits-Rootkits-Development)***: Recopilación (+100) de recursos sobre el desarrollo de bootkits y rootkits.
