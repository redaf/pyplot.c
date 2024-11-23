
#ifndef INCLUDE_PLT_H
#define INCLUDE_PLT_H

#ifdef PLT_STATIC
#define PLT_CDEC static
#else
#define PLT_CDEC extern
#endif

typedef struct
{
    void (*grid)(void);
    void (*plot)(const double *x, const double *y, long int len,
                 const char *fmt);
    void (*show)(void);
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

#define PLT_ASSERT_NON_NULL(ptr, msg)                                          \
    if ((ptr) == NULL)                                                         \
    {                                                                          \
        (void)fprintf(stderr, "ERROR: ");                                      \
        if ((msg) != NULL)                                                     \
        {                                                                      \
            (void)fprintf(stderr, "%s: ", msg);                                \
        }                                                                      \
        (void)fprintf(stderr, "'%s' is null\n", #ptr);                         \
        (void)fflush(stderr);                                                  \
        exit(1);                                                               \
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

static void plt__call_function_1_str(const char *function_name, const char *str)
{
    PyObject *arg = PyUnicode_FromString(str);
    plt__pyobj_check(arg);
    PyObject *result = PyObject_CallOneArg(plt__function(function_name), arg);
    plt__pyobj_check(result);
}

static void plt__grid(void)
{
    PyObject *result = PyObject_CallNoArgs(plt__function("grid"));
    plt__pyobj_check(result);
}

static void plt__show(void)
{
    PyObject *result = PyObject_CallNoArgs(plt__function("show"));
    plt__pyobj_check(result);
}

static PyObject *plt__pylist_from(const double *items, long int len)
{
    PyObject *list = PyList_New(len);
    plt__pyobj_check(list);
    for (long int i = 0; i < len; i++)
    {
        const int res = PyList_SetItem(list, i, PyFloat_FromDouble(items[i]));
        if (res != 0)
        {
            plt__pyerr_print_and_exit();
        }
    }
    return list;
}

static void plt__plot(const double *x, const double *y, long int len,
                      const char *fmt)
{
    PLT_ASSERT_NON_NULL(y, "plt.plot");
    Py_ssize_t count = 1;
    PyObject *x_list = NULL;
    PyObject *y_list = NULL;
    PyObject *fmt_obj = NULL;
    if (x != NULL)
    {
        x_list = plt__pylist_from(x, len);
        plt__pyobj_check(x_list);
        count += 1;
    }
    y_list = plt__pylist_from(y, len);
    plt__pyobj_check(y_list);
    if (fmt != NULL)
    {
        fmt_obj = PyUnicode_FromString(fmt);
        plt__pyobj_check(fmt_obj);
        count += 1;
    }

    PyObject *args = PyTuple_New(count);
    plt__pyobj_check(args);
    count = 0;
    if (x_list != NULL)
    {
        PyTuple_SET_ITEM(args, count, x_list);
        count += 1;
    }
    PyTuple_SET_ITEM(args, count, y_list);
    count += 1;
    if (fmt_obj != NULL)
    {
        PyTuple_SET_ITEM(args, count, fmt_obj);
        count += 1;
    }

    PyObject *result = PyObject_CallObject(plt__function("plot"), args);
    plt__pyobj_check(result);
}

static void plt__title(const char *title)
{
    PLT_ASSERT_NON_NULL(title, "plt.title");
    plt__call_function_1_str("title", title);
}

static void plt__xlabel(const char *xlabel)
{
    PLT_ASSERT_NON_NULL(xlabel, "plt.xlabel");
    plt__call_function_1_str("xlabel", xlabel);
}

static void plt__ylabel(const char *ylabel)
{
    PLT_ASSERT_NON_NULL(ylabel, "plt.ylabel");
    plt__call_function_1_str("ylabel", ylabel);
}

PLT_CDEF plt plt_import(void)
{
    plt__initialize();
    return (plt){
        .grid = plt__grid,
        .plot = plt__plot,
        .show = plt__show,
        .title = plt__title,
        .xlabel = plt__xlabel,
        .ylabel = plt__ylabel,
    };
}

#undef PLT_ASSERT_NON_NULL

#endif // PLT_IMPLEMENTATION
