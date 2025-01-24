# pyplot.c

A c wrapper for [matplotlib.pyplot](https://matplotlib.org/stable/api/pyplot_summary.html).

### Quick start

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
