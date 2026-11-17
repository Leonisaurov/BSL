# Plan: rendimiento, seguridad y usabilidad de libstrings

## Contexto

Estado actual tras la ronda anterior:

- `strings.c` ya maneja `NULL`, overflow y `size_t`.
- `cyclic_shift` ya usa `memmove` y cubre longitudes `0`/`1`.
- Tests cubren casos límite y pasan con `-Werror`, ASan y UBSan.
- Quedan oportunidades claras en:
  - rendimiento: dos `malloc`/`free` por string y copia lineal por repetición;
  - seguridad: APIs que dependen de `strlen` sobre entrada no confiable y falta de validación defensiva en `cyclic_shift`;
  - usabilidad: faltan funciones con longitud explícita y documentación de `errno`.

Decisiones confirmadas:

1. Mantener API/ABI actuales: firmas existentes y layout de `struct string`.
2. Añadir funciones nuevas de forma aditiva.
3. Permitir una única asignación por string; `free_string` liberará un solo bloque.
4. Endurecer el build y eliminar el rpath `$ORIGIN` del binario de test.

## Decisiones de diseño

### API

Mantener:

```c
string_t from_str(const char* str);
string_t repeat_char(char c, size_t r_times);
string_t repeat_str(const char* str, size_t r_times);
void free_string(string_t string);
bool cyclic_shift(string_t string);
```

Añadir:

```c
string_t from_strn(const char* str, size_t len);
string_t repeat_strn(const char* str, size_t str_len, size_t r_times);
```

Semántica:

- `from_strn(NULL, 0)` devuelve un string vacío válido.
- `from_strn(NULL, len > 0)` devuelve `NULL` y deja `errno = EINVAL`.
- `repeat_strn(NULL, 0, r_times)` devuelve un string vacío.
- `repeat_strn(NULL, str_len > 0, ...)` devuelve `NULL` y deja `errno = EINVAL`.
- `from_str` y `repeat_str` siguen usando `strlen` y requieren entrada NUL-terminada; se recomienda `*_n` para datos no confiables o binarios.
- `repeat_str(NULL, ...)` mantiene el comportamiento actual: `NULL` + `EINVAL`.

### Representación interna

Usar una sola asignación por string:

```c
size_t allocation_size = sizeof(struct string) + capacity + 1;
string_t string = malloc(allocation_size);
string->str = (char *)(string + 1);
string->len = 0;
string->size = capacity;
string->str[0] = '\0';
```

- `free_string` hará únicamente `free(string)`.
- Los strings deben crearse con los constructores de la librería y liberarse con `free_string`.
- Documentar que no se debe liberar `string->str` por separado.

### Gestión de errores

Usar `errno` en fallos, sin modificarlo en éxito:

- `EINVAL`: puntero `NULL` con longitud distinta de cero, o `NULL` en `from_str`/`repeat_str`.
- `ENOMEM`: fallo de asignación.
- `ERANGE`: overflow de tamaño o `len > size` en `cyclic_shift`.

`cyclic_shift` devolverá `false` y pondrá `errno` cuando reciba `NULL` o una longitud inconsistente.

### Rendimiento

- `repeat_strn` usará copia exponencial (`memcpy` duplicando el bloque ya escrito) en lugar de una iteración `memcpy` por repetición.
- `cyclic_shift` mantendrá `memmove`.
- Una sola asignación por string reduce llamadas a `malloc`/`free` y mejora localidad.

### Seguridad

- `cyclic_shift` validará `len <= size` antes de tocar memoria.
- Las funciones `*_n` permiten trabajar con longitudes explícitas y evitan `strlen` sobre búferes no confiables.
- Build endurecido:
  - `-D_FORTIFY_SOURCE=2`
  - `-fstack-protector-strong`
  - `-Wl,-z,relro,-z,now`
  - `-Wl,-z,noexecstack`
  - `-pie` para el ejecutable
  - `-Wl,-soname,libstrings.so` para la librería
- Eliminar `-Wl,-rpath,'$ORIGIN'` del binario de test. `make test` usará `LD_LIBRARY_PATH=.` solo para el proceso de test.

### Usabilidad

- Añadir `from_strn` y `repeat_strn` para datos binarios o con NUL embebido.
- Documentar `errno`, hilos, ABI, rendimiento y seguridad en `README.md`.
- Ampliar tests para `errno`, NUL embebido, longitudes explícitas y casos inválidos.

## Tareas ordenadas

1. **`strings.h`**
   - Añadir `from_strn` y `repeat_strn`.
   - Mantener `struct string` y firmas existentes sin cambios.
   - Añadir comentarios breves sobre `errno` y strings creados por la librería.

2. **`strings.c`**
   - Incluir `<errno.h>`.
   - Eliminar `new_string` y su prototipo.
   - Reescribir `allocate_string` para una sola asignación y overflow check:
     `capacity > SIZE_MAX - sizeof(struct string) - 1` -> `errno = ERANGE`.
   - `malloc` fallido -> `errno = ENOMEM`.
   - `free_string` liberará solo `string`.
   - Implementar `from_strn`:
     - `NULL` con `len == 0` -> string vacío.
     - `NULL` con `len > 0` -> `NULL` + `EINVAL`.
     - Copiar `len` bytes y NUL-terminar.
   - `from_str` validará `NULL` y delegará en `from_strn(str, strlen(str))`.
   - Implementar `repeat_char` sobre `allocate_string` + `memset`.
   - Implementar `repeat_strn`:
     - validar `NULL` con `str_len > 0` -> `EINVAL`;
     - comprobar `str_len > SIZE_MAX / r_times` -> `ERANGE`;
     - calcular `total_len`;
     - copiar el primer bloque y luego duplicar con `memcpy` hasta cubrir `total_len`;
     - NUL-terminar y fijar `len`.
   - `repeat_str` validará `NULL` y delegará en `repeat_strn(str, strlen(str), r_times)`.
   - Mantener `free_string(NULL)` como no-op.

3. **`animations.c`**
   - Incluir `<errno.h>`.
   - En `cyclic_shift`:
     - `NULL` o `string->str == NULL` -> `errno = EINVAL`, `false`.
     - `string->len > string->size` -> `errno = ERANGE`, `false`.
     - `len <= 1` -> `true` sin modificar.
     - Usar `memmove` y rotar el primer byte al final.

4. **`main.c`**
   - Incluir `<errno.h>` y `<stdint.h>`.
   - Añadir helpers:
     - `check_null_errno`
     - `check_false_errno`
     - `check_bytes` para comparar `len` y bytes con NUL embebido.
   - Añadir tests:
     - `from_strn("Hello", 5)` -> `"Hello"`.
     - `from_strn("a\0b", 3)` -> longitud 3 y bytes correctos.
     - `from_strn(NULL, 0)` -> vacío.
     - `from_strn(NULL, 1)` -> `NULL` + `EINVAL`.
     - `repeat_strn("ab", 2, 3)` -> `"ababab"`.
     - `repeat_strn("a\0b", 3, 2)` -> longitud 6 y bytes correctos.
     - `repeat_strn(NULL, 0, 5)` -> vacío.
     - `repeat_strn(NULL, 1, 5)` -> `NULL` + `EINVAL`.
     - `repeat_strn("ab", 2, 40000)` -> longitud 80000.
     - `repeat_char('x', SIZE_MAX)` -> `NULL` + `ERANGE`.
     - `repeat_strn("a", 1, SIZE_MAX)` -> `NULL` + `ERANGE`.
     - `cyclic_shift(NULL)` -> `false` + `EINVAL`.
     - `cyclic_shift` con `len > size` -> `false` + `ERANGE`.
     - `free_string(NULL)` no debe fallar.
   - Mantener los tests existentes y el mensaje final `All tests passed`.

5. **`Makefile`**
   - `CPPFLAGS ?= -D_FORTIFY_SOURCE=2`.
   - `CFLAGS ?= -O2 -Wall -Wextra -Wpedantic -Wformat=2 -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wundef -Wpointer-arith -fstack-protector-strong`.
   - Definir hardening condicionado a Linux:
     - `HARDEN_LDFLAGS = -Wl,-z,relro,-z,now -Wl,-z,noexecstack`.
     - `APP_LDFLAGS = -pie`.
     - `SHARED_LDFLAGS = -Wl,-soname,libstrings.so`.
   - Enlace de `libstrings.so`: `-shared`, `SHARED_LDFLAGS`, objetos, `LDFLAGS`, `HARDEN_LDFLAGS`.
   - Enlace de `library_test`: fuentes, `-L. -lstrings`, `APP_LDFLAGS`, `LDFLAGS`, `HARDEN_LDFLAGS`; sin rpath.
   - `test`: `LD_LIBRARY_PATH=. ./$(APP)`.
   - `clean`: eliminar `$(APP)`, `$(LIB_NAME)`, `$(LIB_OBJS)`.

6. **`README.md`**
   - Actualizar API con `from_strn` y `repeat_strn`.
   - Explicar `errno`, strings binarios/NUL embebido y uso de `len`.
   - Documentar que `free_string` libera una sola asignación y que no se debe liberar `string->str` manualmente.
   - Añadir secciones de rendimiento, seguridad, ABI y thread safety.
   - Explicar que `make test` usa `LD_LIBRARY_PATH=.` y que el binario no lleva rpath.
   - Documentar flags de hardening y su orientación a GCC/Clang en Linux.

## Validación

1. Build estricto:
   ```sh
   make clean
   make CFLAGS='-std=c99 -O2 -Wall -Wextra -Wpedantic -Werror'
   ```
2. Tests:
   ```sh
   make test
   ```
3. Sanitizers:
   ```sh
   make clean
   make CFLAGS='-std=c99 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined' \
        LDFLAGS='-fsanitize=address,undefined'
   make test
   ```
4. Comprobar ausencia de rpath/RUNPATH:
   ```sh
   readelf -d library_test | grep -E 'RPATH|RUNPATH' || true
   ```
5. Comprobar hardening de la librería:
   ```sh
   readelf -d libstrings.so | grep -E 'BIND_NOW|FLAGS' || true
   ```
6. Opcional: benchmark manual de `repeat_str("ab", 40000)` para confirmar la mejora de la copia exponencial.

## Riesgos y mitigaciones

- **Strings construidos manualmente**: al usar una sola asignación, `free_string` solo libera el bloque principal. Mitigación: documentar que los strings deben crearse con la librería y liberarse con `free_string`.
- **`from_str`/`repeat_str` siguen usando `strlen`**: no se puede evitar sin romper la API. Mitigación: documentar y ofrecer `*_n` para entrada no confiable.
- **Flags de hardening específicos de Linux/GNU ld**: mitigar con guardas `uname` en el Makefile.
- **`-D_FORTIFY_SOURCE` sin optimización**: el build por defecto usa `-O2`; documentar que sobrescribir `CFLAGS` puede reducir el efecto.
- **`LD_LIBRARY_PATH=.` en tests**: solo se aplica al proceso de test; no se embebe en el binario.

## Fuera de alcance

- No convertir `struct string` en opaco ni cambiar firmas existentes.
- No añadir API de mutación (append, insert, replace).
- No implementar longitud UTF-8 real; el helper antiguo `utf8_string_len` ya no existe.
- No añadir dependencias externas.
- No añadir un target de benchmark permanente.