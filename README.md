# pyplot.c

A limited single file wrapper for [matplotlib.pyplot](https://matplotlib.org/stable/api/pyplot_summary.html).

### Quick start

Simplest example

```c
#define PLT_IMPLEMENTATION
#include "plt.h"

int main()
{
    plt plt = plt_import();

    double y[] = {3, 8, 1, 10, 5, 7};

    plt.plot(NULL, y, 6, NULL, NULL);
    plt.show();
}
```

Compile & run:

```console
$ ls
example.c plt.h
$ cc -I <path/to/Python.h> example.c -o example -L <path/to/libpython3.12.so> -lpython3.12
$ ./example
```

<table>
<tr>
<th>C</th>
<th>Python</th>
</tr>
<tr>
<td>

```c
#include <Python.h>

#define PLT_IMPLEMENTATION
#include "plt.h"

int main()
{
    plt plt = plt_import();
    double xpoints[] = {1, 8};
    double ypoints[] = {3, 10};

    plt.plot(xpoints, ypoints, 2, NULL, NULL);
    plt.show();
}
```

</td>

<td>

```python
import matplotlib.pyplot as plt
import numpy as np

xpoints = np.array([1, 8])
ypoints = np.array([3, 10])

plt.plot(xpoints, ypoints)
plt.show()
```

</td>

</tr>

</table>
