*This project has been created as part of the 42 curriculum by aartyush.*

# get_next_line

## Description

`get_next_line` is a C function that returns **one line at a time** from a file descriptor, so repeated calls in a loop read a whole file (or standard input) line by line.

```c
char *get_next_line(int fd);
```

- **Return value:** the line that was read, *including* the terminating `\n`. The only exception is the last line of a file that does not end with `\n`. Returns `NULL` when there is nothing left to read or when an error occurs.
- **Allowed external functions:** `read`, `malloc`, `free`.
- **Forbidden:** `libft`, `lseek()`, global variables.
- **Goal:** learn how `static` variables keep state between calls, and how to manage heap memory and partial reads safely, whatever the `BUFFER_SIZE`.

### Files

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line()` and its static helpers |
| `get_next_line_utils.c` | `ft_strlen`, `ft_strchr`, `ft_strjoin`, `append_buffer` |
| `get_next_line.h` | prototypes, includes, default `BUFFER_SIZE` |

## Instructions

### Compilation

The code compiles with or without `-D BUFFER_SIZE`. The default value is `42`, defined in `get_next_line.h` when the flag is absent.

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
```

Without the flag the default of 42 is used:

```bash
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c -o gnl
```

### Usage

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test.txt", O_RDONLY);
	if (fd < 0)
		return (1);
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		printf("%s", line);
		free(line);
	}
	close(fd);
	return (0);
}
```

The included `main.c` reads `test.txt`. To read from standard input, pass `0` as the file descriptor.

### Testing suggestions

Try several buffer sizes (`1`, `2`, `42`, `9999`, `10000000`) and several kinds of input:

- an empty file
- a file with only `\n` characters
- a file with no trailing newline
- a very long single line
- standard input (`echo -e "a\nb" | ./gnl`)
- an invalid fd (`-1`, `999`)

Check for leaks and invalid memory access with `valgrind --leak-check=full` or `-fsanitize=address`.

## Algorithm

The function uses one `static char *stash` that survives between calls. It holds data that was already read from the fd but not yet returned to the caller. Each call runs four steps.

1. **Guard.** If `fd < 0` or `BUFFER_SIZE <= 0`, free the stash and return `NULL`.
2. **`read_and_store`: read until a newline is available.** If the stash already contains a `\n`, nothing is read. Otherwise `read()` is called with `BUFFER_SIZE` bytes into a heap buffer. Each chunk is appended to the stash with `append_buffer`, which allocates a new string and copies the old stash plus the new chunk. The loop stops as soon as the chunk contains a `\n`, at EOF (`read` returns `0`), or on error (`-1`). On error the stash is freed and `NULL` is returned.
3. **`extract_line`: copy the line.** Copy everything from the start of the stash up to and including the first `\n`, or up to the end of the stash if there is no `\n`. If the stash is empty, return `NULL`.
4. **`reposition_stash`: keep the remainder.** Allocate a new stash with whatever follows the returned line and free the old one. If there is no `\n` (last line, EOF), the stash is freed and set to `NULL`.

### Why this design

- **Reads as little as possible.** The function stops reading at the first chunk that contains a `\n`, and never loads the whole file, as the subject asks.
- **Works for any `BUFFER_SIZE`.** Whether it is 1 (many small reads) or 10,000,000 (one huge read), anything read past the newline stays in the stash for the next call. The buffer is allocated on the heap, so a large `BUFFER_SIZE` does not overflow the stack.
- **Works on any fd.** Only `read()` is used, so regular files, pipes and standard input behave the same way.
- **Safe memory handling.** The stash is the only thing that persists. It is freed at EOF, on read errors and on allocation failures, and `free_and_null` resets pointers to avoid double frees. The caller frees each returned line.
- **Avoids rescanning and re-measuring.** `append_buffer` receives the already known lengths of the stash and the chunk, so it never recomputes them while the loop is running.

### Known limitations

- A single static stash means the mandatory version tracks **one** fd at a time. Interleaving reads on several fds is the bonus part.
- Appending creates a new string each time. For extremely long lines combined with a tiny `BUFFER_SIZE` this costs O(n²/BUFFER_SIZE) copying. This is fine for normal input.
- Reading binary files (NUL bytes) and modifying the file between calls are undefined behaviour, per the subject.

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`
- Static variables and storage duration in C (e.g. cppreference: *Storage-class specifiers*)
- 42 Norm and the *get_next_line* subject (version 14.3)
- Valgrind and AddressSanitizer documentation, for leak and memory error checks

### Use of AI

AI (Claude) was used to draft this README and to review the finished implementation. The review covered edge cases such as different buffer sizes, empty files, missing trailing newlines, invalid fds and memory-leak checks.