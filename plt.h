
#ifndef INCLUDE_PLT_H
#define INCLUDE_PLT_H

#ifdef PLT_STATIC
#define PLT_CDEC static
#else
#define PLT_CDEC extern
#endif

// API

typedef struct
{
    void (*grid)(void);
    void (*plot)(const double *x, const double *y, int len, const char *fmt,
                 ...);
    void (*scatter)(const double *x, const double *y, int len);
    void (*show)(void);
    void (*subplot)(int nrows, int ncols, int index);
    void (*title)(const char *title);
    void (*xlabel)(const char *xlabel);
    void (*ylabel)(const char *ylabel);
} plt;

PLT_CDEC plt plt_import(void);

#endif // INCLUDE_PLT_H

#ifdef PLT_IMPLEMENTATION

#include <stdio.h>  // fprintf, fflush
#include <stdlib.h> // exit

#ifdef PLT_STATIC
#define PLT_CDEF static
#else
#define PLT_CDEF
#endif

// API functions declaration

static void plt__grid(void);
static void plt__plot(const double *x, const double *y, int len,
                      const char *fmt, ...);
static void plt__scatter(const double *x, const double *y, int len);
static void plt__show(void);
static void plt__subplot(int nrows, int ncols, int index);
static void plt__title(const char *title);
static void plt__xlabel(const char *xlabel);
static void plt__ylabel(const char *ylabel);

// Private functions declaration

static void plt__assert_non_null(const void *ptr, const char *msg,
                                 const char *ptr_name);
static void plt__function_call_1_str(const char *function_name,
                                     const char *str);
static void plt__initialize(void);
static void plt__pyerr_print_and_exit(void);
static void plt__pyobj_check(PyObject *obj);
static void plt__py_plot(PyObject *x, PyObject *y, PyObject *fmt,
                         PyObject *kwargs);

static PyObject *plt__function(const char *name);
static PyObject *plt__module(void);
static PyObject *plt__pydict_from(va_list list);
static PyObject *plt__pylist_from(const double *items, Py_ssize_t len);

#define PLT__ASSERT_NON_NULL(ptr, msg) plt__assert_non_null((ptr), (msg), #ptr)

// API functions definition

PLT_CDEF plt plt_import(void)
{
    plt__initialize();
    return (plt){
        .grid = plt__grid,
        .plot = plt__plot,
        .scatter = plt__scatter,
        .show = plt__show,
        .subplot = plt__subplot,
        .title = plt__title,
        .xlabel = plt__xlabel,
        .ylabel = plt__ylabel,
    };
}

static void plt__grid(void)
{
    PyObject *result = PyObject_CallNoArgs(plt__function("grid"));
    plt__pyobj_check(result);
}

static void plt__plot(const double *x, const double *y, int len,
                      const char *fmt, ...)
{
    PLT__ASSERT_NON_NULL(y, "plt.plot");
    PyObject *x_list = NULL;
    PyObject *y_list = NULL;
    PyObject *fmt_uni = NULL;
    PyObject *kwargs_dict = NULL;

    if (x != NULL)
    {
        x_list = plt__pylist_from(x, (Py_ssize_t)len);
        plt__pyobj_check(x_list);
    }

    y_list = plt__pylist_from(y, (Py_ssize_t)len);
    plt__pyobj_check(y_list);

    if (fmt != NULL)
    {
        fmt_uni = PyUnicode_FromString(fmt);
        plt__pyobj_check(fmt_uni);
    }

    va_list kw_list;
    va_start(kw_list, fmt);
    kwargs_dict = plt__pydict_from(kw_list);
    va_end(kw_list);

    plt__py_plot(x_list, y_list, fmt_uni, kwargs_dict);
}

static void plt__scatter(const double *x, const double *y, int len)
{
    PLT__ASSERT_NON_NULL(x, "plt.scatter");
    PLT__ASSERT_NON_NULL(y, "plt.scatter");
    PyObject *x_list = plt__pylist_from(x, (Py_ssize_t)len);
    PyObject *y_list = plt__pylist_from(y, (Py_ssize_t)len);
    PyObject *result =
        PyObject_CallFunction(plt__function("scatter"), "(OO)", x_list, y_list);
    plt__pyobj_check(result);
}

static void plt__show(void)
{
    PyObject *result = PyObject_CallNoArgs(plt__function("show"));
    plt__pyobj_check(result);
}

static void plt__subplot(int nrows, int ncols, int index)
{
    PyObject *result = PyObject_CallFunction(plt__function("subplot"), "iii",
                                             nrows, ncols, index);
    plt__pyobj_check(result);
}

static void plt__title(const char *title)
{
    PLT__ASSERT_NON_NULL(title, "plt.title");
    plt__function_call_1_str("title", title);
}

static void plt__xlabel(const char *xlabel)
{
    PLT__ASSERT_NON_NULL(xlabel, "plt.xlabel");
    plt__function_call_1_str("xlabel", xlabel);
}

static void plt__ylabel(const char *ylabel)
{
    PLT__ASSERT_NON_NULL(ylabel, "plt.ylabel");
    plt__function_call_1_str("ylabel", ylabel);
}

// Private functions definition

static void plt__assert_non_null(const void *ptr, const char *msg,
                                 const char *ptr_name)
{
    if (ptr != NULL)
    {
        return;
    }

    (void)fprintf(stderr, "ERROR: ");
    if ((msg) != NULL)
    {
        (void)fprintf(stderr, "%s: ", msg);
    }
    (void)fprintf(stderr, "'%s' is null\n", ptr_name);
    (void)fflush(stderr);
    exit(1);
}

static void plt__function_call_1_str(const char *function_name, const char *str)
{
    PyObject *arg = PyUnicode_FromString(str);
    plt__pyobj_check(arg);
    PyObject *result = PyObject_CallOneArg(plt__function(function_name), arg);
    plt__pyobj_check(result);
}

static void plt__initialize(void)
{
    if (Py_IsInitialized())
    {
        return;
    }
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        plt__pyerr_print_and_exit();
    }
}

static void plt__pyerr_print_and_exit(void)
{
    PyErr_Print();
    exit(1);
}

static void plt__pyobj_check(PyObject *obj)
{
    if (obj == NULL)
    {
        plt__pyerr_print_and_exit();
    }
}

static void plt__py_plot(PyObject *x, PyObject *y, PyObject *fmt,
                         PyObject *kwargs)
{
    Py_ssize_t args_count = 1; // y cannot be null
    Py_ssize_t args_index = 0;
    PyObject *args = NULL;
    PyObject *result = NULL;

    args_count += x != NULL;
    args_count += fmt != NULL;
    args = PyTuple_New(args_count);
    plt__pyobj_check(args);

    // Add x
    if (x != NULL)
    {
        PyTuple_SET_ITEM(args, args_index, x);
        args_index += 1;
    }
    // Add y
    PyTuple_SET_ITEM(args, args_index, y);
    args_index += 1;
    // Add fmt
    if (fmt != NULL)
    {
        PyTuple_SET_ITEM(args, args_index, fmt);
        args_index += 1;
    }

    result = PyObject_Call(plt__function("plot"), args, kwargs);
    plt__pyobj_check(result);
}

static PyObject *plt__function(const char *name)
{
    PyObject *function = PyObject_GetAttrString(plt__module(), name);
    plt__pyobj_check(function);
    if (!PyCallable_Check(function))
    {
        fprintf(stderr, "ERROR: '%s' is not a callable object\n", name);
        exit(1);
    }
    return function;
}

static PyObject *plt__module(void)
{
    static PyObject *module = NULL;
    if (module != NULL)
    {
        return module;
    }
    module = PyImport_ImportModule("matplotlib.pyplot");
    plt__pyobj_check(module);
    return module;
}

static PyObject *plt__pydict_from(va_list list)
{
#define PLT__VA_ARGS_PAIRS_LIMIT 64

    PyObject *dict = NULL;
    PyObject *key_uni = NULL;
    PyObject *val_uni = NULL;
    const char *key = NULL;
    const char *val = NULL;

    dict = PyDict_New();
    plt__pyobj_check(dict);

    for (Py_ssize_t i = 0; i < PLT__VA_ARGS_PAIRS_LIMIT; i++)
    {
        key = va_arg(list, const char *);
        val = va_arg(list, const char *);
        if ((key == NULL) || (val == NULL))
        {
            break;
        }
        key_uni = PyUnicode_FromString(key);
        val_uni = PyUnicode_FromString(val);
        plt__pyobj_check(key_uni);
        plt__pyobj_check(val_uni);
        if (PyDict_SetItem(dict, key_uni, val_uni) != 0)
        {
            plt__pyerr_print_and_exit();
        }
    }

    return dict;
#undef PLT__VA_ARGS_PAIRS_LIMIT
}

static PyObject *plt__pylist_from(const double *items, Py_ssize_t len)
{
    PyObject *list = PyList_New(len);
    plt__pyobj_check(list);
    for (Py_ssize_t i = 0; i < len; i++)
    {
        if (PyList_SetItem(list, i, PyFloat_FromDouble(items[i])) != 0)
        {
            plt__pyerr_print_and_exit();
        }
    }
    return list;
}

#undef PLT__ASSERT_NON_NULL

#endif // PLT_IMPLEMENTATION
